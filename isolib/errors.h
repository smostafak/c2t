#ifndef ERRORS_H_
#define ERRORS_H_
#include "iso8583.h"
#define	ERR_OVRLEN	100
#define	ERR_OUTMEM	101
#define  ERR_IVLFLD		102

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
static const errmsg errdef[] ={
		{1, "The length of field is too long"}, /*The length of field is too long*/
		{2, "The length of field is not correct"}, /*The length of field is not correct*/
		{3, "The format of this field is not define"},  /*The format of this field is not define*/
		{4, "The index of field is out of range"},	/*The index of field is out of range*/
		{5,"The value of this field is not correct format"}, /*The value of this field is not correct format*/
		{6, "Out of memory"} /*Out of memory*/
}; /*The format error*/

/*!	\func	void err_iso(int *fldErr, FILE *fp)
 * 		\brief this function is call to write the errors in msg to the log file
 * 		\param	fldErr is an array that contains all errors in msg 8583
 * 		\filename is name of log file
*/
void err_iso(int *fldErr, char *filename);
//char *scan_err(int err_code, int fld_idx)
/*!	\func	char *scan_err(int *fldErr, FILE *fp)
 * 		\brief	This function is used to show the description of errors
 * 		\param	err_code is the return value of the function iso8583_set_fmtbitmap
 * 		\Output: one message to description the error
 */
char *scan_err(int err_code);

/*!	\func	check_fld(char *value, int idx, cons isodef *def)
 * 		\brief	this procedure is call to check weather a field is correct or not (pack and unpack)
 * 		\param	value: the value of this field (is set or get from msg)
 * 		\param idx: the possition of this field in msg
 * 		\param def: the definition is used to parse msg
 * 		\output: the errors code if have or zero (not error)
*/
int check_fld(char *value, int idx, const isodef *def);

/*!	\fn	void *err_sys(int err_code, FILE *fp)
 * 		\brief	This function is used to process the system error (such as out of memory ...)
 * 		\param	err_code is the code of this error
 * 		\param filename is the name of the log file for system error
 * 		\Output: the desc of this error is written to log file (file name)
 */
void err_sys(int err_code, char *filename);
#endif /*ERRORS_H_*/
