/*!	\file		errors.c
 * 		\brief	This file contains all functions that support main iso-related functions
 *		\All functin in this file use to process errors in iso message
 */

#include	 <stdio.h>
#include	 <stdlib.h>
#include	 <string.h>
#include	 <time.h>
#include	 <ctype.h>
#include "errors.h"
#include "iso8583.h"

/*!	\fn	void iso_err(int *fldErr, char *filename)
 *  	\brief	Show the message error to the logfile.
 * 		\param fldErr: the array contains all errors in message
 * 		\param filename: the name of log file
 * 		\return the log file write error in message
 */

void iso_err(int *fldErr, char *filename)
{
	time_t t;
	int i, j, nerr;
	char* tmp;
    FILE *fp;
    fp = fopen(filename, "a+");
    //nerr = sizeof(errdef)/sizeof(errmsg);
    if (!fp)
    {
    	printf("Can not open file %s", filename);
    	exit(1);
    }
    t = time(0);
    tmp = ctime(&t);
    tmp[strlen(tmp)-1] = '\0';
    fprintf(fp, "<parse_time %s>\n", tmp);
    for(i = 0; i < 129; i++)
    {
    	if (fldErr[i] != 0)
    	{
    		j = 0;
    		while (j < nerr)
    		{
    			if (fldErr[i] == errdef[j].Err_ID)
    			{

    				fprintf(fp, "	<field id = %d>\n", i);
    				fprintf(fp, "		<err_code>%d>", fldErr[i]);
    				fprintf(fp, "		</err_code>\n");
    				fprintf(fp, "		<desc>%s>", errdef[i].dsc);
    				fprintf(fp, "		</desc>\n");
    				fprintf(fp, "	</field>\n", i);
    				break;
    			}
    			j++;
    		}
    	}
    }
    fprintf(fp, "</parse_time>\n");
    fclose(fp);
}

/*!	\fn	char *scan_err(int *fldErr, FILE *fp)
* 		\brief	this procedure is call when having error during field setting
* 		\param	err_code is the return value of the function iso8583_set_fmtbitmap
* 		\Output: description about the error
*/

char* scan_err(int err_code)
{
	int i, nerr;
	char *desc;
	i = 0;
	nerr = sizeof(errdef)/sizeof(errmsg);
	for(; i< nerr; i++)
	{
		if (errdef[i].Err_ID == err_code)
		{
			desc = (char *) malloc(strlen(errdef[i].dsc) * sizeof(char));
			strcpy(desc, errdef[i].dsc);
			break;
		}
	}
	if (!desc)
	{
		return "Can not recognize this error code";
	}
	else
	{
		return desc;
	}
}

/*!	\fn	check_fld(char *value, int idx, cons isodef *def)
 * 		\brief	this procedure is call to check weather a field is correct or not (pack and unpack)
 * 		\param	value: the value of this field (is set or get from msg)
 * 		\param idx: the possition of this field in msg
 * 		\param def: the definition is used to parse msg
 * 		\output: the errors code if have or zero (not error)
*/
int check_fld(char *value, int idx, const isodef* def)
{
	int i, err_code;
	int lengthfld; //The length of the value


	lengthfld = strlen(value);
	if (def[idx].format == ISO_BITMAP)
	{
		err_code = 0;
	}
	else if (def[idx].format == ISO_NUMERIC)
	{
		for (i = 0; i < lengthfld; i++)
		{
			if (isdigit(value[i]) == 0)
			{
				err_code = 5;
				break;
			}
		}
	}
	else if (def[idx].format == ISO_ALPHANUMERIC)
	{
		for (i = 0; i < lengthfld; i++)
		{
			if (isalnum(value[i]) == 0)
			{
				err_code = 5;
				break;
			}
		}
	}
	else if (def[idx].format == ISO_BINARY)
	{
		for (i = 0; i < lengthfld; i++)
		{
			if ((value[i] != '0') && (value[i] != '1'))
			{
				err_code = 5;
				break;
			}
		}
	}
	else
	{
		//format of this field is not defined
		err_code = 3;
	}
	return err_code;
}
/*!	\fn	void *sys_err(int err_code, FILE *fp)
 * 		\brief	This function is used to process the system error (such as out of memory ...)
 * 		\param	err_code is the code of this error
 * 		\param filename is the name of the log file for system error
 * 		\Output: the desc of this error is written to log file (file name)
 */
void sys_err(int err_code, char *filename)
{
	time_t t;
	char *desc;
    FILE *fp;
    fp = fopen(filename, "a+");
    if (!fp)
    {
    	printf("Can not open file %s", filename);
    	exit(1);
    }
    t = time(0);
    fprintf(fp, "%s\n", ctime(&t));
    desc = scan_err(err_code);
    fprintf(fp, "%s\n", desc);
    fclose(fp);
}
/*! \fn void handle_err(int err_code, int err_type, char *desc)
 * 		\brief This function is used to write the error of one field to the log file
 * 		\param err_code the error code that recieved when error appear
 * 		\param err_type is type of error, if err_type = 1 is the system error else is the iso error
 * 		\param desc is the description of this error of developer
 * 		\output: the desc of this error is written to log file (file name)
 */

int handle_err(int err_code, int err_type, char *moredesc)
{
	char *filename;
	char *str; //Stirng of time
	FILE *fp;
	time_t t;
	struct tm *ptr;
	char *desc;
	filename="log.txt";
	t = time(0);
	ptr = localtime(&t);
	fp = fopen(filename, "a+");
	if (!fp)
	{
		printf("Can not open file %s", filename);
		return -1;
	}
	desc = scan_err(err_code);
	if (strcmp(desc, "Can not recognize this error code") == 0)
	{
		return -1;
	}
    //fprintf(fp, "%s -",  ctime(&t));
    strftime(str, 100, "%d-%m-%Y:%H:%M:%S", ptr);
    fprintf(fp, "%s -", str);
    if (err_type == ISO)
    	fprintf(fp, "%d - ISO - %s - %s\n", err_code, desc, moredesc);
    else
    	fprintf(fp, "%d - SYS - %s - %s\n", err_code, desc, moredesc);
    fclose(fp);
    return 0;
}