/*!	\file		iso8583.h
 * 		\brief	The header file contain all structure of this library
 */
#ifndef _ISO8583_H
#define _ISO8583_H 1

#include <stdio.h>
#include <time.h>

#define ISO_BITMAP       0
#define ISO_NUMERIC      1
#define ISO_ALPHANUMERIC 2
#define ISO_BINARY       3

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

/*!	\struct		errmsg
 * 		\brief		The ERROR ISO message structure 		
 */	
typedef struct {
	/*!  \brief  The error format:
	 * 			- 0 for 
	 * 			- 1 for 
	 * 			- 2 for 
	 * 			- 3 for 
	 */ 
	int Err_ID;
	
	/*! \brief This var represents the description of this error */
	const char *dsc;	
} errmsg;
/*Define the const error of iso message*/
const errmsg errdef[] ={
		{1, ""}, /*The length of field is to long*/
		{2, ""}, /*The length of field is not correct*/
		{3, ""}}; /*The format error*/
void iso8583_init(isomsg *m);	
int iso8583_set_fmtbitmap(isomsg *m, int bmp_flag);
int iso8583_get_bitmap(isomsg *m, isodef* def, char *buf);
int iso8583_pack(const isomsg *m, const isodef *d, char *buf);
int iso8583_unpack(isomsg *m, const isodef *d, const char *buf);
void isoerrreport(int *fldErr, FILE *fp);
void iso8583_dump(FILE *fp, isomsg *m);
void iso8583_free(isomsg *m);

#endif /* iso8583.h */
