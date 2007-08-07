/*!	\file	iso8583.c
 * 		\brief	The main source file of this library
 */

#include <stdlib.h>
#include <string.h>
#include "iso8583.h"
#include "utilities.h"
#include "errors.h"

/*!	\func	int setdata(isomsg *m, char *buff );
 * 		\brief	set data to a field
 * 		\param	m is a pointer to a isomsg
 * 		\param  buff is a pointer to byte string
 */
static int set_data(isomsg *m, int idx, char *buff )
{
    if (idx >= 1 && idx <= 129) {
    	int len = strlen(buff);
    	m->fld[idx] = (char *)malloc((len + 1)*sizeof(char));
    	if (m->fld[idx] == NULL) return ERR_OUTMEM;
    	memcpy(m->fld[idx], buff, len);
    }
    return 0;
}

/*!	\func	void init_message(isomsg *m);
 * 		\brief	Initialize an ISO message struct - i.e. set all entries to NULL
 * 		\param	m is an ::isomsg
 */
void init_message(isomsg *m, int bmp_flag)
{
	int i;
	m->bmp_flag = bmp_flag; 				/*!	Default format is binary format */
	for (i = 0; i <= 128; i++) {
		m->fld[i] = NULL;
	}
}


/*!	\func 	int pack_message(const isomsg *m, const isodef *d, char *buf)
 *		\brief  Using the definition d, pack the content of the ISO message m into buf. \n
 * 				 NOTE: buf must be large enough to contain the packed message.
 *				   Returns the length of the data written to buf.
 *
 * 		\param		m is an ::isomsg structure pointer that contains all message elements to be packed
 * 		\param 	d is an array of ::isodef structures which refers to all data element definitions of  an iso standard
 * 		\param		buf is the iso message buffer that contains the packed iso message.
 * 		\return		The length of the iso message buffer or zero if have errors.
 */
int pack_message(const isomsg *m, const isodef *d, char *buf)
{
	char *start = buf;
	int flds;
	int i, sys_err_code;
	char *bitmap;
	int len;
	char tmp[20];
	Bytes byte_tmp, hexa_tmp;
	int flderr[129];

	if (strlen(m->fld[0]) != d[0].flds || d[0].lenflds != 0) {
		/* FIXME: error */
		/*This error is the length of field is not correct*/
		handle_err(ERR_IVLLEN, ISO, "Field 0 --> Length is over its length in iso8583 definition");
		return 0;
	}
	memcpy(buf, m->fld[0], d[0].flds);
	buf += d[0].flds;

	/*
	 * The bitmap contains either 64 or 128 fields - flds is the
	 * number of allowed fields, i.e. either 64 or 128.
	 */
	flds = 64;
	for (i = 65; i <= 128; i++) {
		if (m->fld[i] != NULL) {
			flds = 128;
			break;
		}
	}

	bitmap = (char *) calloc(flds/8 + 1, sizeof(char));
	if (!bitmap)
	{
		char err_msg[100];
		sprintf("%s:%d --> Can't allocate memory for the bitmap buffer", __FILE__, __LINE__);
		handle_err(ERR_OUTMEM, SYS, err_msg);
		return 0;
	}
	/*
	 * First bit in the bitmap (field 1) defines if the message is
	 * extended or not.
	 * 0: not exteded - i.e. only (some of) the fields 0 to 64 are
	 *    used
	 * 1: extended - i.e. (some of) the fields 0 to 128 are used
	 * I.e. if one or more of the fields 65 to 128 is used, the
	 * bit is 1.
	 */
	if (flds == 128) {
		bitmap[0] |= 0x80;
	}

	/*
	 * The bits 2 to 64/128 defines if the corresponding field is
	 * present (1) or not (0).
	 */
	for (i = 2; i <= flds; i++) {
		if (m->fld[i] != NULL) {
			/* Set the i'th bit to 1 */
			bitmap[(i-1)/8] |= 0x80 >> ((i-1)%8);
		}
	}

	if(m->bmp_flag == BMP_BINARY){
		memcpy(buf, bitmap, flds/8);
		buf += flds/8;
	}else{
		byte_tmp.length = flds;
		byte_tmp.bytes = (char*) calloc(byte_tmp.length, sizeof(char));
		if(!byte_tmp.bytes){
			char err_msg[100];
			sprintf("%s:%d --> Can't allocate memory", __FILE__, __LINE__);
			handle_err(ERR_OUTMEM, SYS, err_msg);
			free(bitmap);
			return 0;
		}
		memcpy(byte_tmp.bytes, bitmap, byte_tmp.length/8);
		if( bytes2hexachars(&byte_tmp, &hexa_tmp) < 0){
			char err_msg[100];
			sprintf("%s:%d --> Can't convert bytes to a hexacharacter array", __FILE__, __LINE__);
			handle_err(ERR_BYTHEX, SYS, err_msg);
			free(bitmap);
			free(byte_tmp.bytes);
			return 0;
		};
		memcpy(buf, hexa_tmp.bytes, hexa_tmp.length);
		buf += hexa_tmp.length;
		free(byte_tmp.bytes);
		free(hexa_tmp.bytes);
	}

	for (i = 2; i <= flds; i++) {
		if ((bitmap[(i-1)/8] << ((i-1)%8)) & 0x80) { /* i'th bit != 0 */
			if (d[i].lenflds) { /* Variable length */
				switch (d[i].format) {
				case ISO_NUMERIC: /* Fallthrough */
				case ISO_ALPHANUMERIC:
					len = strlen(m->fld[i]);
					sprintf(tmp, "%%0%dd", d[i].lenflds);
					sprintf(buf, tmp, len);
					if (len > d[i].flds) {
						/*The length of this field is too long*/
						char err_msg[100];
						sprintf(err_msg, "Field %d --> The length of this field is too long", i);
						handle_err(ERR_OVRLEN, ISO, err_msg);
						return 0;
					}
					buf += d[i].lenflds;
					break;
				case ISO_BINARY:
					sprintf(tmp, "%%0%dd", d[i].lenflds);
					sscanf(m->fld[i], tmp, &len);
					if (len > d[i].flds) {
						char err_msg[100];
						sprintf(err_msg, "Field %d --> The length of this field is too long", i);
						handle_err(ERR_OVRLEN, ISO, err_msg);
						return 0;
					}
					/* Copy length bytes from m->fld[i] */
					len += d[i].lenflds;
					break;
				default:{
						/* FIXME: error */
						/*The format error of this field*/
						/*Format out of range*/
						char err_msg[100];
						sprintf(err_msg, "Field %d --> Format out of range", i);
						return 0;
						break;
					}
				}
			} else { /* Fixed length */
				len = d[i].flds;

				/* FIXME: How can we check ISO_BINARY? */
				if (d[i].format != ISO_BINARY &&
				    strlen(m->fld[i]) != len) {
					/* FIXME: error */
					/*The lengthe is not correct*/
					char err_msg[100];
					sprintf(err_msg, "Field %d --> The length is not correct", i);
					return 0;
				}
			}
			if (flderr[i] == 0)
			{
				flderr[i] = check_fld(m->fld[i], i, d);
			}

			memcpy(buf, m->fld[i], len);

			buf += len;
                }
	}
	return (buf - start);
}


/*!	\func			int unpack_message(isomsg *m, const isodef *d, const char *buf)
 * 		\brief 		Using the definition d, unpack the content of buf into the ISO message struct m.
 * 		\param 	m is an ::isomsg structure pointer that contains all message elements which are unpacked
 * 		\param 	d is an array of ::isodef structures which refers to all data element definitions of  an iso standard
 * 		\param		buf is the iso message buffer that contains the iso message that needs unpacking.
 * 		\returns    0 in case successful unpacking \n
 * 						error number in case an error occured
 */
int  unpack_message(isomsg *m, const isodef *d, const char *buf)
{
	int flds;
	int i, sys_err_code;
	int len;
    char tmp[20];
    Bytes byte_tmp, hexa_tmp;
    int flderr[129];

	/* Field 0 is mandatory and fixed length. */
	if (d[0].lenflds != 0) {
		/* FIXME: error */
		/*The length define is not correct*/
		handle_err(ERR_IVLLEN, ISO, "Definition 0 --> The length define is not correct");
		return 0;
	}
	m->fld[0] = (char *) malloc((d[0].flds + 1) * sizeof(char));
	if(!m->fld[0])
	{
		char err_msg[100];
		sprintf(err_msg, "%s:%d --> Can't allocate memory for field 0", __FILE__, __LINE__);
		handle_err(ERR_OUTMEM, SYS, err_msg);
		return 0;
	}
	memcpy(m->fld[0], buf, d[0].flds);
	m->fld[0][d[0].flds] = 0;
	buf += d[0].flds;

	/*
	 * First bit in the bitmap (field 1) defines if the message is
	 * extended or not.
	 * 0: not exteded - i.e. only (some of) the fields 0 to 64 are
	 *    used
	 * 1: extended - i.e. (some of) the fields 0 to 128 are used
	 * I.e. if one or more of the fields 65 to 128 is used, the
	 * bit is 1.
	 */
	if(m->bmp_flag == BMP_BINARY){
		if(buf[0] & 0x80) {
			flds = 128;
		} else {
			flds = 64;
		}
		m->fld[1] = (char *) calloc(flds/8 + 1, sizeof(char));
		if (!m->fld[1])
		{
			char err_msg[100];
			sprintf(err_msg, "%s:%d --> Can't allocate memory for field 1", __FILE__, __LINE__);
			handle_err(ERR_OUTMEM, SYS, err_msg);
			return 0;
		}
		memcpy(m->fld[1], buf, flds/8);
		buf += flds/8;
	}else{
		if(hexachar2int(buf[0]) & 0x08){
			flds = 128;
		} else {
			flds = 64;
		}
		hexa_tmp.length = flds/4;
		hexa_tmp.bytes = (char *) calloc(hexa_tmp.length, sizeof(char));
		memcpy(hexa_tmp.bytes, buf, flds/4);
		if( hexachars2bytes(&hexa_tmp, &byte_tmp) < 0){
			handle_err(ERR_HEXBYT, SYS, "Can't convert the bitmap hexachar array to binary");
			free(hexa_tmp.bytes);
			free(m->fld[0]);
			return 0;
		}

		m->fld[1] = (char*) calloc(byte_tmp.length/8 + 1, sizeof(char));
		if (!m->fld[1])
		{
			char err_msg[100];
			sprintf(err_msg, "%s:%d --> Can't allocate memory for field 1", __FILE__, __LINE__);
			handle_err(ERR_OUTMEM, SYS, err_msg);
			free(m->fld[0]);
			free(hexa_tmp.bytes);
			free(byte_tmp.bytes);
			return 0;
		}
		memcpy(m->fld[1], byte_tmp.bytes, byte_tmp.length/8);
		buf += flds/4;
		free(hexa_tmp.bytes);
		free(byte_tmp.bytes);
	}

	for (i = 2; i <= flds; i++) {
		if ((m->fld[1][(i-1)/8] << ((i-1)%8)) & 0x80) { /* i'th bit != 0 */
			if (d[i].lenflds) { /* Variable length */
				sprintf(tmp, "%%0%dd", d[i].lenflds);
				sscanf(buf, tmp, &len);
				/*
				 * The length of a field can't be
				 * larger than defined by d[i].flds.
				 */
				if (len > d[i].flds) {
					/* FIXME: warning/error */
					/*The length of this field is too long*/
					char err_msg[100];
					sprintf(err_msg, "Field %d --> The length of this field is too long", i);
					handle_err(ERR_OVRLEN, ISO, err_msg);
					return 0;
				}
			} else { /* Fixed length */
				len = d[i].flds;
			}

			switch (d[i].format) {
			case ISO_NUMERIC: /* Fallthrough */
			case ISO_ALPHANUMERIC:
				/* Don't copy lenght bytes */
				buf += d[i].lenflds;
				break;
			case ISO_BINARY:
				/* Copy length bytes */
				len += d[i].lenflds;
				break;
			default:{
					/* FIXME: error */
					/*The format of this field is not defined*/
					char err_msg[100];
					sprintf(err_msg, "Field %d --> The format of this field is not defined", i);
					handle_err(ERR_IVLFMT, ISO, err_msg);
					return 0;
					break;
				}
			}
			m->fld[i] = (char *) malloc((len + 1) * sizeof(char));
			if (!m->fld[i])
			{
				char err_msg[100];
				sprintf(err_msg, "%s:%d --> Can't allocate memory for field %d", __FILE__, __LINE__, i);
				handle_err(ERR_OUTMEM, SYS, err_msg);
				return 0;
			}
			memcpy(m->fld[i], buf, len);
			m->fld[i][len] = 0;
			//Check the value of this field correct format or not
			if (flderr[i] == 0)
				flderr[i] = check_fld(m->fld[i], i, d);
			buf += len;
		}
	}
	return 0;
}


/*!	\func 			void dump_message(FILE *fp, isomsg *m)
 * 		\brief 		Dump the content of the ISO message m into a file
 * 		\param 	fp is a FILE pointer that points to the message-storing file
 * 		\param		m is an ::isomsg structure pointer that contains all message elements which needs dumping
 */

void dump_message(FILE *fp, isomsg *m)
{
	int i;

	for (i = 0; i <= 128; i++) {
		if (m->fld[i] != NULL) {
			fprintf(fp, "field #%d = %s\n", i, m->fld[i]);
		}
	}
}


/*!	\func			void free_message(isomsg *m)
 *  	\brief		Free memory used by the ISO message struct m.
 */
void free_message(isomsg *m)
{
	int i;

	for (i = 0; i <= 128; i++) {
		if (m->fld[i] != NULL) {
			free(m->fld[i]);
			m->fld[i] = NULL;
		}
	}
}
/*!	\func			int set_field(isomsg* m, const isodef *def, int idx, const char* fld)
 *  	\brief		set data to a field of iso msg.
 */
/*!	\func			int set_field(isomsg* m, const isodef *def, int idx, const char* fld)
 *  	\brief		set data to a field of iso msg.
 */

int set_field(isomsg* m, const isodef *def, int idx, const char* fld)
{
	char err_msg[100];
	
	if ((idx > 128) || (idx < 0)) {
		/*
		 * The value of idx must be between 0 and 128
		 */
		sprintf(err_msg, "%s:%d --> Invalid field %d", __FILE__, __LINE__, idx);
		handle_err(ERR_IVLFLD, ISO, err_msg);
		return ERR_IVLFLD; /*Invalid field*/
	}

	int format = def[idx].format;
	int len = strlen(fld);

	if (len > def[idx].flds) {
		/*
		 * The length of this field is too long
		 */
		sprintf(err_msg, "%s:%d --> The length of field %d is too long", __FILE__, __LINE__, idx);
		handle_err(ERR_OVRLEN, ISO, err_msg);
		return ERR_OVRLEN;
	}
	
	int err_code = check_fld(fld, idx, def);
	if (err_code) {
		/*
		 * The value of this field is invalid
		 */
		sprintf(err_msg, "%s:%d --> The value of field %d is invalid", __FILE__, __LINE__, idx);
		handle_err(err_code, ISO, err_msg);
		return err_code;
	}
	
	m->fld[idx] = (char *)malloc((len + 1)*sizeof(char));
	if (m->fld[idx] == NULL) {
		sprintf(err_msg, "%s:%d --> Can't allocate memory for field %d", __FILE__, __LINE__,idx);
		handle_err(ERR_OUTMEM, SYS, err_msg);
		return ERR_OUTMEM;
	}
	
	memcpy(m->fld[idx], fld, len);
	m->fld[idx][len] = '\0';
	
	switch (format) {
		case ISO_BITMAP:			
			break;
		case ISO_NUMERIC:
			if (IS_FIXED_LEN(def,idx)) {
				lpad(m->fld[idx], def[idx].flds, '0');
			}
			break;
		case ISO_ALPHANUMERIC:
			if (IS_FIXED_LEN(def,idx)) {
				rpad(m->fld[idx], def[idx].flds, ' ');
			}
			break;
		case ISO_BINARY:
			break;
		default:
			sprintf(err_msg, "Field %d --> Format out of range", idx);
			handle_err(ERR_IVLFMT, ISO, err_msg);
			return ERR_IVLFMT; /*invalid format*/
	}
	return 0;
}

/*!	\func			int get_field(const char* buf, const isodef *def, int idx, char* fld, int bmp_flag);
 *  	\brief		get data of a field from the msg buff.
 */

int get_field(char *buf, const isodef *def, int idx, char *fld, int bmp_flag)
{
	char *pos = &buf[0];

	char err_msg[100];
		
	if ((idx > 128) || (idx < 0)) {
		/*
		 * The value of idx must be between 0 and 128
		 */
		sprintf(err_msg, "%s:%d --> Invalid field %d", __FILE__, __LINE__, idx);
		handle_err(ERR_IVLFLD, ISO, err_msg);
		return ERR_IVLFLD; /*Invalid field*/
	}

	/* Field 0 is mandatory and fixed length. */
	if (def[0].lenflds != 0) {		
		/*The length define is not correct*/
		handle_err(ERR_IVLLEN, ISO, "Definition 0 --> The length define is not correct");
		return 0;
	}

	if (idx == 0) {
		/* the field is mti */
		memcpy(fld,pos,def[0].flds);
		fld[def[0].flds] = 0;
		return 0;
	}

	pos += def[0].flds;
	/* get the bitmap to determine a field exist in the msg */
	/*
	 * First bit in the bitmap (field 1) defines if the message is
	 * extended or not.
	 * 0: not exteded - i.e. only (some of) the fields 0 to 64 are
	 *    used
	 * 1: extended - i.e. (some of) the fields 0 to 128 are used
	 * I.e. if one or more of the fields 65 to 128 is used, the
	 * bit is 1.
	 */
	int flds = 64;
	char bitmap[100];

	if(bmp_flag == BMP_BINARY){
		if(*pos & 0x80) {
			flds = 128;
		} else {
			flds = 64;
		}
		memcpy(bitmap, pos, flds/8);
		bitmap[flds/8] = 0;
		pos += flds/8;
	}else{
		if(hexachar2int(*pos) & 0x08) {
			flds = 128;
		} else {
			flds = 64;
		}
		Bytes byte_tmp, hexa_tmp;
		hexa_tmp.length = flds/4;
		hexa_tmp.bytes = (char *) calloc(hexa_tmp.length, sizeof(char));
		if (hexa_tmp.bytes == NULL) {
			sprintf(err_msg, "%s:%d --> Can't allocate memory", __FILE__, __LINE__);
			handle_err(ERR_OUTMEM, SYS, err_msg);
			return ERR_OUTMEM;
		}
		memcpy(hexa_tmp.bytes, pos, flds/4);
		if( hexachars2bytes(&hexa_tmp, &byte_tmp) < 0){
			handle_err(ERR_HEXBYT, SYS, "Can't convert the bitmap hexachar array to binary");
			free(hexa_tmp.bytes);			
			return ERR_HEXBYT;
		}
		
		memcpy(bitmap, byte_tmp.bytes, byte_tmp.length/8);
		free(hexa_tmp.bytes);
		free(byte_tmp.bytes);
		bitmap[byte_tmp.length/8] = 0;
		pos += flds/4;
	}

	if (idx == 1) {
		/* the field is bitmap */
		int len = strlen(bitmap);
		memcpy(fld,bitmap,len);
		fld[len] = 0;
		return 0;
	}

	if (idx <= flds && ((bitmap[(idx-1)/8] << ((idx-1)%8)) & 0x80)) { /* idx'th bit != 0 */
		int i;
		int len;
		char tmp[20];
		for (i = 2; i <= idx; i++) {
			if ((bitmap[(i-1)/8] << ((i-1)%8)) & 0x80) { /* i'th bit != 0 */
				if (!IS_FIXED_LEN(def,i)) { /* Variable length */
					sprintf(tmp, "%%0%dd", def[i].lenflds);
					sscanf(pos, tmp, &len);
					/*
					 * The length of a field can't be
					 * larger than defined by def[i].flds.
					 */
					if (len > def[i].flds) {
						sprintf(err_msg, "%s:%d --> The length of field %d is too long", __FILE__, __LINE__, idx);
						handle_err(ERR_OVRLEN, ISO, err_msg);
						return ERR_OVRLEN;
					}
				}
				else { /* Fixed length */
					len = def[i].flds;
				}
				switch (def[i].format) {
					case ISO_NUMERIC: /* Fallthrough */
					case ISO_ALPHANUMERIC:
						/* Don't copy lenght bytes */
						pos += def[i].lenflds;
						break;
					case ISO_BINARY:
						/* Copy length bytes */
						len += def[i].lenflds;
						break;
					default:
						sprintf(err_msg, "Field %d --> Format out of range", idx);
						handle_err(ERR_IVLFMT, ISO, err_msg);
						return ERR_IVLFMT; /*invalid format*/
				}

				if (i == idx) {
					memcpy(fld, pos, len);
					fld[len] = '\0';
				}
				pos += len;
			}
		}
	}
	else {
		/* the field not exist */
		/*sprintf(err_msg, "The fied %d is not exist", idx);
		handle_err(ERR_IVLFLD, ISO, err_msg);*/
		return ERR_IVLFLD; /*invalid format*/
	}
	return 0;
}
