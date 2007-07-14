#ifndef ERRORS_H_
#define ERRORS_H_
#define	ERR_OVRLEN	100	//The length of field is to long
#define	ERR_IVLPAR	110	//invalid parameter (eg. set int value to a alphanmeric field
#define	ERR_IVLFLD	111	//invalid field
#define	ERR_OUTMEM	120	//Out of memory
/*Define the const error of iso message*/
const errmsg errdef[] ={
		{1, ""}, /*The length of field is to long*/
		{2, ""}, /*The length of field is not correct*/
		{3, ""}
}; /*The format error*/
#endif /*ERRORS_H_*/
