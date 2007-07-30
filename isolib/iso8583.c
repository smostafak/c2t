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
void init_message(isomsg *m)
{
	int i;
	m->bmp_flag = 0; 				/*!	Default format is binary format */
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

	char *filename, *syslog;
	filename = "./log.txt";
	syslog = "./syslog.txt";

	/* Field 0 is mandatory and fixed length. */
	           for (i = 0; i < 129; i++)
	           {
	        	   flderr[i] = 0;
	           }
	if (strlen(m->fld[0]) != d[0].flds || d[0].lenflds != 0) {
		/* FIXME: error */
		/*This error is the length of field is not correct*/
		flderr[0] = ERR_IVLLEN;
		iso_err(flderr, filename);
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
		sys_err_code = 6;
		sys_err(sys_err_code, syslog);
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
			sys_err_code = ERR_OUTMEM;
			sys_err(sys_err_code, syslog);
			free(bitmap);
			return 0;
		}
		memcpy(byte_tmp.bytes, bitmap, byte_tmp.length/8);
		if( bytes2hexachars(&byte_tmp, &hexa_tmp) < 0){
			flderr[1] = ERR_BYTHEX;
			iso_err(flderr, filename);
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
						/* FIXME: error */
						/*The length of this field is too long*/
						flderr[i] = ERR_OVRLEN;
						iso_err(flderr, filename);
						return 0;
					}
					buf += d[i].lenflds;
					break;
				case ISO_BINARY:
					sprintf(tmp, "%%0%dd", d[i].lenflds);
					sscanf(m->fld[i], tmp, &len);
					if (len > d[i].flds) {
						/* FIXME: error */
						/*The length of field is too long*/
						flderr[i] = ERR_OVRLEN;
						iso_err(flderr, filename);
						return 0;
					}
					/* Copy length bytes from m->fld[i] */
					len += d[i].lenflds;
					break;
				default:
					/* FIXME: error */
					/*The format error of this field*/
					/*Format out of range*/
					flderr[i] = ERR_IVLFMT;
					iso_err(flderr, filename);
					return 0;
					break;
				}
			} else { /* Fixed length */
				len = d[i].flds;

				/* FIXME: How can we check ISO_BINARY? */
				if (d[i].format != ISO_BINARY &&
				    strlen(m->fld[i]) != len) {
					/* FIXME: error */
					/*The lengthe is not correct*/
					flderr[i] = ERR_IVLLEN;
					iso_err(flderr, filename);
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
	/*write all error in this message to file*/
	iso_err(flderr, filename);
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
    char *filename;
    char *syslog;
    Bytes byte_tmp, hexa_tmp;
    int flderr[129];

	for (i = 0; i < 129; i++)
	{
		flderr[i] = 0;
	}
	filename = "./log.txt";
	syslog = "./syslog.txt";
	/* Field 0 is mandatory and fixed length. */
	if (d[0].lenflds != 0) {
		/* FIXME: error */
		/*The length define is not correct*/
		flderr[0] = ERR_IVLLEN;
		iso_err(flderr, filename);
		return 0;
	}
	m->fld[0] = (char *) malloc((d[0].flds + 1) * sizeof(char));
	if(!m->fld[0])
	{
		sys_err_code = ERR_OUTMEM;
		sys_err(sys_err_code, syslog);
		free(m->fld[0]);
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
	if(buf[0] & 0x80) {
		flds = 128;
	} else {
		flds = 64;
	}

	if(m->bmp_flag == BMP_BINARY){
		m->fld[1] = (char *) calloc(flds/8 + 1, sizeof(char));
		if (!m->fld[1])
		{
			sys_err_code = ERR_OUTMEM;
			//void sys_err(int err_code, char *filename);
			sys_err(sys_err_code, syslog);
			return 0;
		}
		memcpy(m->fld[1], buf, flds/8);
		buf += flds/8;
	}else{
		hexa_tmp.length = flds/4;
		hexa_tmp.bytes = (char *) calloc(hexa_tmp.length, sizeof(char));
		memcpy(hexa_tmp.bytes, buf, flds/4);
		if( hexachars2bytes(&hexa_tmp, &byte_tmp) < 0){
			flderr[1] = ERR_HEXBYT;
			iso_err(flderr, filename);
			free(hexa_tmp.bytes);
			free(m->fld[0]);
			return 0;
		}

		m->fld[1] = (char*) calloc(byte_tmp.length/8 + 1, sizeof(char));
		if (!m->fld[1])
		{
			sys_err_code = ERR_OUTMEM;
			sys_err(sys_err_code, syslog);
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
					flderr[i] = ERR_OVRLEN;
					iso_err(flderr, filename);
					free(buf);
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
			default:
				/* FIXME: error */
				/*The format of this field is not defined*/
				flderr[i] = ERR_IVLFMT;
				iso_err(flderr, filename);
				return 0;
				break;
			}
			m->fld[i] = (char *) malloc((len + 1) * sizeof(char));
			if (!m->fld[i])
			{
				sys_err_code = ERR_OUTMEM;
				//void sys_err(int err_code, char *filename);
				sys_err(sys_err_code, syslog);
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
	for (i = 0; i < 129; i++)
		if (flderr[i] !=0)
		{
			iso_err(flderr, filename);
			break;
		}
	//return set_data(m, idx, numchar);
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

	if ((idx > 128) || (idx < 0)) {
		/*
		 * The value of idx must be between 0 and 128
		 */
		return ERR_IVLFLD; /*Invalid field*/
	}

	int format = def[idx].format;
	int len = strlen(fld);

	if (len > def[idx].flds) {
		/*
		 * The length of this field too long
		 */
		return ERR_OVRLEN;
	}

	switch (format) {
		case ISO_BITMAP:
			m->fld[idx] = (char *)malloc((len + 1)*sizeof(char));
			if (m->fld[idx] == NULL)
				return ERR_OUTMEM;

			memcpy(m->fld[idx], fld, len);
			m->fld[idx][len] = '\0';
			break;
		case ISO_NUMERIC:
			m->fld[idx] = (char *)malloc((len + 1)*sizeof(char));
			if (m->fld[idx] == NULL)
				return ERR_OUTMEM;

			memcpy(m->fld[idx], fld, len);
			m->fld[idx][len] = '\0';
			if (IS_FIXED_LEN(def,idx)) {
				lpad(m->fld[idx], def[idx].flds, '0');
			}
			break;
		case ISO_ALPHANUMERIC:
			m->fld[idx] = (char *)malloc((len + 1)*sizeof(char));
			if (m->fld[idx] == NULL)
				return ERR_OUTMEM;

			memcpy(m->fld[idx], fld, len);
			m->fld[idx][len] = '\0';
			if (IS_FIXED_LEN(def,idx)) {
				rpad(m->fld[idx], def[idx].flds, ' ');
			}
			break;
		case ISO_BINARY:
			m->fld[idx] = (char *)malloc((len + 1)*sizeof(char));
			if (m->fld[idx] == NULL)
				return ERR_OUTMEM;

			memcpy(m->fld[idx], fld, len);
			m->fld[idx][len] = '\0';
			break;
		default:
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

	if ((idx > 128) || (idx < 0)) {
		/*
		 * The value of idx must be between 0 and 128
		 */
		return ERR_IVLFLD; /*Invalid field*/
	}

	/* Field 0 is mandatory and fixed length. */
	if (def[0].lenflds) {
		/* FIXME: error */
		return -1;
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
	if(*pos & 0x80) {
		flds = 128;
	} else {
		flds = 64;
	}
	char bitmap[100];

	if(bmp_flag == BMP_BINARY){
		memcpy(bitmap, pos, flds/8);
		bitmap[flds/8] = 0;
		pos += flds/8;
	}else{
		Bytes byte_tmp, hexa_tmp;
		hexa_tmp.length = flds/4;
		hexa_tmp.bytes = (char *) calloc(hexa_tmp.length, sizeof(char));
		memcpy(hexa_tmp.bytes, pos, flds/4);
		hexachars2bytes(&hexa_tmp, &byte_tmp);
		memcpy(bitmap, byte_tmp.bytes, byte_tmp.length/8);
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
		return -1;
	}
	return 0;
}
