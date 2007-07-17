/*!	\file		errors.c
 * 		\brief	This file contains all functions that support main iso-related functions
 *		\All functin in this file use to process errors in iso message
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "errors.h"
/*!	\fn			isoerrreport(int *fldErr, FILE *fp)
 *  	\brief		Show the message error to the logfile. 
 * 		\param fldErr: the array contains all errors in message
 * 		\the pointer fp point to a log file
 * 		\Output: the log file write error in message
 */
void err_iso(int *fldErr, FILE *fp)
{
	time_t t;
	int i, j;
    t = time(0);

    fprintf(fp, "The debug error for day: %s", ctime(&t));
    for(i=0; i<129; i++)
    {
    	if (fldErr(i) != 0)
    	{

    		j = 0;
    		while (errdef(j) != null)
    		{
    			if (fleErr[i] == errdef(j))
    			{
    				fprintf(fp, "Error appear on the field %d is: %s", i, errdef[j].desc);
    				break;
    			}
    			j = j + 1;
    		}
    	    
    	}
    }
    fclose(fp);
}

/*!	\fn	void err_iso(int *fldErr, FILE *fp)		 
 * 		\brief	this procedure is call when having error during field setting
 * 		\param	err_code is the return value of the function iso8583_set_fmtbitmap
 * 		\param  fld_idx point to the field error 
 * 		\Output: description about the error
 */

char *err_field(int err_code, int fld_idx)