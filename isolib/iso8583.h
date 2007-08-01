/*!	\file		iso8583.h
 * 		\brief	The header file contain all structure of this library
 */
#ifndef _ISO8583_H
#define _ISO8583_H

#include <stdio.h>
#include <time.h>

#define ISO_BITMAP       0
#define ISO_NUMERIC      1
#define ISO_ALPHANUMERIC 2
#define ISO_BINARY       3


#define IS_FIXED_LEN(def,idx) (def[(idx)].lenflds==0)


#define BMP_BINARY		0
#define BMP_HEXA			1


/*!	\struct		isodef
 * 		\brief		The structure holds all data delement definitions of an iso8583 version.
 */
typedef struct {
	/*!  \brief  The data element format:
	 * 			- 0 for BITMAP
	 * 			- 1 for Numeric
	 * 			- 2 for alphanumeric
	 * 			- 3 for binary
	 */
	int format;

	/*! \brief  This var represents the length of the len portion of this data element (LL or LLL) */
	int lenflds;

	/*! \brief This var represents the len of the data element */
	int flds;

	/*! \brief This var represents the description of this data element */
	const char *dsc;
} isodef;

/*!	\struct		isomsg
 * 		\brief		The ISO message structure
 */
typedef struct {
	/*! \brief The flag to identify whether the bitmap is in binary or hexa format */
	int bmp_flag;
	/*! \brief The 129 field pointer array		*/
	char *fld[129];
} isomsg;

void init_message(isomsg *m, int bmp_flag);
int pack_message(const isomsg *m, const isodef *d, char *buf);
int unpack_message(isomsg *m, const isodef *d, const char *buf);
void dump_message(FILE *fp, isomsg *m);
void free_message(isomsg *m);

/*!	\func	set data to a field of iso msg	*/
int set_field(isomsg* m, const isodef *def, int idx, const char* fld);
/*!	\func	get data from a field of iso msg	*/
int get_field(char* buf, const isodef *def, int idx, char* fld, int bmp_flag);


#endif /* iso8583.h */
