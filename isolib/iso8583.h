/*!	\file		iso8583.h
 * 		\brief	The header file contain all structure of this library
 */
#ifndef _ISO8583_H
#define _ISO8583_H

#include <stdio.h>
#include <time.h>
#include "utilities.h"

#define ISO_BITMAP       					0			/*!	\brief	Bitmap	datatype */
#define ISO_NUMERIC      					1			/*!	\brief 	N datatype */
#define ISO_ALPHABETIC					2			/*!	\brief	A datatype */
#define ISO_BINARY       					3			/*!	\brief 	B datatype */
#define ISO_Z									4			/*!	\brief	Z datatype */
#define ISO_ALPHANUMERIC 				5			/*!	\brief 	AN datatype	*/
#define ISO_ALPHASPECIAL 				6			/*!	\brief	AS datatype */
#define ISO_NUMERICSPECIAL			7			/*!	\brief	NS datatype */
#define ISO_XNUMERIC  					8			/*!	\brief 	X+Numeric datatype of field, X is 'C' or 'D' */
#define ISO_ALPHANUMERIC_PAD		9 			/*!	\brief	ANP datatype */
#define ISO_ALPHANUMERIC_SPC		10		/*!	\brief	ANS datatype	*/


#define ISO_MAX_LENGTH		8192
#define XML_MAX_LENGTH	8192
#define PLAIN_MAX_LENGTH	8912
#define FIELD_MAX_LENGTH	1024

#define IS_FIXED_LEN(def,idx) (def[(idx)].lenflds==0)


#define BMP_BINARY		0
#define BMP_HEXA			1

#define FMT_PLAIN		0
#define FMT_XML		1

#define ISO_VER87	0
#define ISO_VER93	1

#define XML_ROOT_TAG		"isomsg"
#define XML_CHILD_TAG		"field"
#define XML_FIELD_INDEX		"id"
#define XML_FIELD_VALUE	"value"


/*!	\struct		isodef
 * 		\brief		The structure holds all data delement definitions of an iso8583 version.
 */
typedef struct {
	/*!  \brief  The data element format */
	int format;

	/*! \brief  This var represents the length of the len portion of this data element (LL or LLL) */
	int lenflds;

	/*! \brief This var represents the len of the data element */
	int flds;

	/*! \brief This var represents the description of this data element */
	const char *dsc;
} isodef;

/*!	\struct		isodef
 * 		\brief		The structure holds all data delement definitions of an iso8583 version.
 */
typedef struct {
	/*!  \brief  The message properties:
	/*! \brief The flag to identify whether the bitmap is in binary or hexa format */
	int bmp_flag;
	/*! \brief The padding character for alphanumeric fields */
	char alphanumeric_pad;
	/*! \brief The padding character for numeric fields */
	char numeric_pad;
} msgprop;

/*!	\struct		isomsg
 * 		\brief		The ISO message structure
 */
typedef struct {
	/*! \brief Properties of this iso message */
	msgprop prop;
	/*! \brief The iso definition that the fields of this iso message conform to */
	const isodef *def;
	/*! \brief The 129 field pointer array, each memeber cotains a byte array and its length */
	bytes fld[129];
} isomsg;

 /*! 		\brief	Initialize an ISO message struct - i.e. set all entries to NULL */
void init_message(isomsg *m, const isodef *def, const msgprop *prop);

/*!	\brief  pack the content of an ISO message  into a buffer. \n
 * 				 NOTE: the buffer must be large enough to contain the packed message.
 */
int pack_message(isomsg *m, char **buf, int *buf_len);

 /*! 		\brief 		Using the definition d, unpack the content of buf into the ISO message struct m. */
int unpack_message(isomsg *m, const char *buf, int buf_len);

void dump_message(FILE *fp, isomsg *m, int fmt_flag);

/*!  	\brief		Free memory used by the ISO message struct m. */
void free_message(isomsg *m);

/*! 	\brief	assign an iso definition to m */
void set_isodef(isomsg *m, isodef *def);

/*! 	\brief	assign an msgprop to m */
void set_prop(isomsg *m, msgprop *prop);

/*!	\brief	convert an iso message to xml format		*/
char* iso_to_xml(char* iso_msg, int iso_len, const isodef* def ,msgprop* prop);

/*!	\brief	convert an xml string to iso message		*/
char* xml_to_iso(char* xml_str, const isodef *def, msgprop* prop, int* iso_len);

/*!	\func	set data to a field of iso msg	*/
int set_field(isomsg* m, int idx, const char *fld, int fld_len);
/*!	\func	get data from a field of iso msg	*/
int get_field(char* buf, const isodef *def, const msgprop *prop, int idx, char *fld, int *fld_len);

#endif /* iso8583.h */
