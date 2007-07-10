/*!	\file	iso8583.c
 * 		\brief	The main source file of this library 
 */
#include <stdlib.h> 
#include <string.h>
#include "iso8583.h"

/*!	\fn	void iso8583_init(isomsg *m);			 
 * 		\brief	Initialize an ISO message struct - i.e. set all entries to NULL
 * 		\param	m is an ::isomsg   
 */
void iso8583_init(isomsg *m)	
{
	int i;

	for (i = 0; i <= 128; i++) {
		m->fld[i] = NULL;
	}
}

/*!	\fn 	int iso8583_fmtbitmap(isomsg *m, int bmp_flag)
 *		\brief  set the bitmap flag for the isomsg struct m
 *  
 * 		\param		m is an ::isomsg structure pointer that contains all message elements to be packed or unpacked
 * 		\param		bmp_flag is the flag used to identify whether the bitmap is in hexa format (bmp_flag=1) or binary format (bmp_flag=0) 
 * 		\return		error code
 */ 
int iso8583_set_fmtbitmap(isomsg *m, int bmp_flag){
}

/*!	\fn 	int iso8583_get_bitmap(isomsg *m, char *buf, char* bitmap)
 *		\brief  This function used to get the bitmap out of the message buffer
 * 
 * 		\param		m is an ::isomsg structure pointer that contains all message elements to be packed or unpacked
 * 		\param		buf is the iso message buffer that contains the packed iso message.
 * 		\param		bimap is the output bitmap 
 * 		\return		error code
 */
int iso8583_get_bitmap(isomsg *m, char *buf){
}

/*!	\fn 	int iso8583_pack(const isomsg *m, const isodef *d, char *buf)
 *		\brief  Using the definition d, pack the content of the ISO message m into buf. \n
 * 				 NOTE: buf must be large enough to contain the packed message.
 *				   Returns the length of the data written to buf.
 * 
 * 		\param		m is an ::isomsg structure pointer that contains all message elements to be packed
 * 		\param 	d is an array of ::isodef structures which refers to all data element definitions of  an iso standard
 * 		\param		buf is the iso message buffer that contains the packed iso message. 
 * 		\return		The length of the iso message buffer.
 */
int iso8583_pack(const isomsg *m, const isodef *d, char *buf)
{
	char *start = buf;
	int flds;
	int i;
	char *bitmap;
	int len;
	char tmp[20];

	/* Field 0 is mandatory and fixed length. */
	if (strlen(m->fld[0]) != d[0].flds || d[0].lenflds != 0) {
		/* FIXME: error */
		/*This error is the length of field is too long*/
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
	memcpy(buf, bitmap, flds/8);
	buf += flds/8;

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
					}
					buf += d[i].lenflds;
					break;
				case ISO_BINARY:
					sprintf(tmp, "%%0%dd", d[i].lenflds);
					sscanf(m->fld[i], tmp, &len);
					if (len > d[i].flds) {
						/* FIXME: error */
					}
					/* Copy length bytes from m->fld[i] */
					len += d[i].lenflds;
					break;
				default:
					/* FIXME: error */
					/*The format error of this field*/
					/*Format out of range*/
					break;
				}
			} else { /* Fixed length */
				len = d[i].flds;

				/* FIXME: How can we check ISO_BINARY? */
				if (d[i].format != ISO_BINARY && 
				    strlen(m->fld[i]) != len) {
					/* FIXME: error */
				}
			}

			memcpy(buf, m->fld[i], len);

			buf += len;
                }
	}

	return (buf - start);
}


/*!	\fn			int iso8583_unpack(isomsg *m, const isodef *d, const char *buf)
 * 		\brief 		Using the definition d, unpack the content of buf into the ISO message struct m.
 * 		\param 	m is an ::isomsg structure pointer that contains all message elements which are unpacked
 * 		\param 	d is an array of ::isodef structures which refers to all data element definitions of  an iso standard
 * 		\param		buf is the iso message buffer that contains the iso message that needs unpacking.
 * 		\returns    0 in case successful unpacking \n
 * 						error number in case an error occured
 */
int iso8583_unpack(isomsg *m, const isodef *d, const char *buf)
{
	int flds;
	int i;
	int len;
        char tmp[20];

	/* Field 0 is mandatory and fixed length. */
	if (d[0].lenflds != 0) {
		/* FIXME: error */
	}
	m->fld[0] = (char *) malloc((d[0].flds + 1) * sizeof(char));
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
	m->fld[1] = (char *) calloc(flds/8 + 1, sizeof(char));
	memcpy(m->fld[1], buf, flds/8);
	buf += flds/8;

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
				break;
			}

			m->fld[i] = (char *) malloc((len + 1) * sizeof(char));
			memcpy(m->fld[i], buf, len);
			m->fld[i][len] = 0;

			buf += len;
		}
	}

	return 0;
}


/*!	\fn 			void iso8583_dump(FILE *fp, isomsg *m)
 * 		\brief 		Dump the content of the ISO message m into a file
 * 		\param 	fp is a FILE pointer that points to the message-storing file
 * 		\param		m is an ::isomsg structure pointer that contains all message elements which needs dumping 
 */	
 	
void iso8583_dump(FILE *fp, isomsg *m)
{
	int i;

	for (i = 0; i <= 128; i++) {
		if (m->fld[i] != NULL) {
			fprintf(fp, "field #%d = %s\n", i, m->fld[i]);
		}
	}
}


/*!	\fn			void iso8583_free(isomsg *m)
 *  	\brief		Free memory used by the ISO message struct m. 
 */
void iso8583_free(isomsg *m)
{
	int i;

	for (i = 0; i <= 128; i++) {
		if (m->fld[i] != NULL) {
			free(m->fld[i]);
			m->fld[i] = NULL;
		}
	}
}
/*!	\fn			isoerrreport(int *fldErr, FILE *fp)
 *  	\brief		Show the message error to the logfile. 
 */
void isoerrreport(int *fldErr, FILE *fp)
{
    time_t t;
    int i, j;
    t = time(0);
    fprintf(fp, "The debug error for day: %s", ctime(&t));
    for(i=0; i<129; i++)
    {
    	if fldErr(i) != 0 then
    	{
    		/*Searching for the desc of this error*/
    		j = 0;
    		while (errdef(j) != null)
    		{
    			if fleErr[i] = errdef(j)
    			{
    				fprintf(fp, "Error appear on the field %d is: %s", i, errdef[j].desc);
    				break;
    			}
    			j = j + 1;
    		}
    	    
    	}
    }
    fclose(fp)
}

