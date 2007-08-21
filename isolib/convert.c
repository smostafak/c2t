/*!	\file	convert.c
 * 		\brief	This file contains all converting utilities of this iso8583 library
 */
#include <stdio.h>
#include <stdlib.h>
#include	 <string.h>
#include <errno.h>
#include "expat.h"
#include "convert.h"
#include "errors.h"
#include "iso8583.h"



int Depth;
XML_Parser parser;
static int err_no;

/*!	\func		char* iso_to_xml(char* iso_msg)
 * 		\brief		convert a message in iso format to xml format
 * 		\param		iso_msg	a character pointer that points to this message
 * 		\param		ios_len		the length of the iso message
 * 		\param 	def is an array of ::isodef structures which refers to all data element definitions of  an iso standard
 * 		\param		bmp_flag  a flag that indicates which fomat the bitmap is packaged (binary, hexa)
 * 		\return		a xml string if successfully convert the message	\n
 * 						NULL in case having an error
 */
char* iso_to_xml(char* iso_msg, int iso_len, const isodef* def, int bmp_flag){
	char* xml_str; // xml string buffer
	isomsg unpacked_msg;
	char  tmp[FIELD_MAX_LENGTH];
	int err = 0, i = 0;
	init_message(&unpacked_msg, bmp_flag);
	err = unpack_message(&unpacked_msg, def, iso_msg, iso_len);
	if(err > 0){
		handle_err(WARN, ISO, "Can not unpack the iso message");
		return NULL;
	}else{
		char *tail;
		xml_str = (char*) calloc(XML_MAX_LENGTH, sizeof(char));
		tail = xml_str;
		sprintf(tail, "<?xml\tversion=\"1.0\"?>\n<%s>\n", XML_ROOT_TAG);
		tail = xml_str + strlen(xml_str);
		for(i = 0; i <= 128; i++){
			if (i == 1) continue;
			if (unpacked_msg.fld[i] != NULL){
				memset(tmp, '\0', sizeof(tmp));
				sprintf(tmp, "\t<%s\tid=\"%d\"\tvalue=\"%s\"/>\n", XML_CHILD_TAG, i, unpacked_msg.fld[i]);
				if(strlen(tmp) + strlen(xml_str) <= XML_MAX_LENGTH){
					sprintf(tail, "%s", tmp);
					tail = xml_str + strlen(xml_str);
				}else{
					handle_err(ERR_OVRLEN, SYS, "The xml string's length exceeds the defined maximum value");
					free(xml_str);
					return NULL;
				}
			}
		}
		memset(tmp, '\0', sizeof(tmp));
		sprintf(tmp, "</%s>", XML_ROOT_TAG);
		if(strlen(tmp) + strlen(xml_str) <= XML_MAX_LENGTH){
				sprintf(tail, "%s", tmp);
		}else{
				handle_err(ERR_OVRLEN, SYS, "The xml string's length exceeds the defined maximum value");
				free(xml_str);
				return NULL;
		}
		return xml_str;
	}
}

/*!	\func		char* xml_to_iso(char* xml_str, const isodef *def);
 * 		\brief		convert a xml string to an iso message
 * 		\param		xml_str the xml input string
 * 		\param 	def is an array of ::isodef structures which refers to all data element definitions of  an iso standard
 * 		\param		bitmap flag that will be used to build the iso message
 * 		\param		iso_len the output iso message's length
 * 		\return 	 	the iso message string if having no error
 * 						NULL if having an error
 */
 char* xml_to_iso(char* xml_str, const isodef *def, int bmp_flag, int* iso_len){
 	int done=0;
 	char* current_pos = xml_str;
 	char iso_buf[ISO_MAX_LENGTH];
 	int len = 0;
 	isomsg	iso_msg;
	int err = 0;
	/* reset the err_no */
	err_no = 0;
 	parser = XML_ParserCreate(NULL);

 	init_message(&iso_msg, bmp_flag);

 	XML_SetUserData(parser, &iso_msg);

 	memset(iso_buf, '\0', sizeof(iso_buf));

	if (! parser) {
		char	err_msg[100];
		sprintf(err_msg, "At file: %s		line:%s", __FILE__, __LINE__);
		handle_err(ERR_PASMEM, SYS, err_msg);
		return NULL;
  }
	XML_SetElementHandler(parser, handle_start, handle_end);

	for(;;){
		len = (strlen(current_pos) < TMPSIZE)? strlen(current_pos): TMPSIZE;
		if(len < TMPSIZE) done = 1;
		if (XML_Parse(parser, current_pos, len, done) == XML_STATUS_ERROR) {
		  char err_msg[100];
	      sprintf(err_msg, "Parse error at line %" XML_FMT_INT_MOD "u:\n%s\n",
	              XML_GetCurrentLineNumber(parser),
	              XML_ErrorString(XML_GetErrorCode(parser)));
	         handle_err(ERR_XMLPAS, ISO, err_msg);
	      return NULL;
	    }
	    current_pos += len;
	    if(*current_pos=='\0') break;
	}
	/* free allocated resources, and return	*/
	XML_ParserFree(parser);

	if(err_no){
		free_message(&iso_msg);
		/* reset the erro_no before return */
		err_no = 0;
		return NULL;
	}else{
		err = pack_message(&iso_msg, def, iso_buf, iso_len);
		iso_buf[*iso_len] = '\0';
		free_message(&iso_msg);
		/* reset the erro_no before return */
		err_no = 0;
		return iso_buf;
	}
 }

/*!
 * 		\brief	handle xml start tag
 */
static void XMLCALL
handle_start(void *data, const char *el, const char **attr)
{
  int i = 0;
  int fld_index = -1;
  char* fld_data = NULL;
  /* check err_no */
  if(err_no) return;
  isomsg* tmp = (isomsg*)data;
  if(strcmp(el, XML_CHILD_TAG) ==0){
		while(attr[i]){
			if((strcmp(attr[i], XML_FIELD_INDEX) == 0) && (i%2 == 0) ){	/* found the index attribute */
				if(!attr[i+1]) continue;		/* don't have a value for 'index' attribute, continue */
				if(fld_index >= 0) continue;		/* There are two 'index' attribute, ormit the second one*/
				fld_index = strtol(attr[i+1],(char**)NULL, 10);
				if(errno){
					fld_index = -1;
				}else{
					if( fld_index < 0 || fld_index > 128 || fld_index == 1){ /* the index value is not correct */
						char	err_msg[100];
						sprintf(err_msg, "The index value(%d) is not in the [0, 128] range and <> 1", fld_index);
						handle_err(ERR_OUTRAG, ISO, err_msg);
						fld_index = -1;
					}
				}
			}else{
				if((strcmp(attr[i], XML_FIELD_VALUE) ==0) && (i%2 ==0)){	/* found the value attribute */
					if(!attr[i+1]) continue;		/*	don't have a value for 'value' attribute, continue */
					if(fld_data) continue;			/* There are two 'value' attribute, ormit the second one*/
					fld_data = (char*) calloc(strlen(attr[i+1])+1, sizeof(char));
					memcpy(fld_data, attr[i+1], strlen(attr[i+1]));
				}
			}
			i+=2;
		}
		if( (fld_index >= 0) && fld_data){
			/*	having both the field index and the field value, set them to the isomsg struct */
			tmp->fld[fld_index] = (char*) calloc(strlen(fld_data)+1, sizeof(char));
			memcpy(tmp->fld[fld_index], fld_data, strlen(fld_data));
		}else{
			char err_msg[100];
			sprintf(err_msg, "Syntax error at line: %d of the parsing xml document, either index attribute or value attribute is not correct", XML_GetCurrentLineNumber(parser));
			handle_err(WARN,ISO, err_msg);
			err_no = ERR_XMLSYT;
		}
  }
  Depth++;
}

/*!
 * 		\brief	handle xml end tag
 */
static void XMLCALL
handle_end(void *data, const char *el)
{
  Depth--;
}
