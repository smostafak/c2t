/*!	\file		utilities.c
 * 		\brief	This file contains all functions that support main iso-related functions
 */
#include "utilities.h"
#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
/*!	\fn	int hexachar2int(char hexa_char)
 * 		\brief	This function convert a hexa character to its correspondent integer value
 * 		\param		hexa_char	the character to convert
 *		\param		ptrint	the pointer that contains the address of  the output int.
 * 		\return		SUCCEEDED if having no error \n
 * 						error number if having an error
 */
int	hexachar2int(char hexa_char, int* ptrint){
	setlocale(LC_CTYPE, "C");
	if( isxdigit(hexa_char)){
		if( isdigit(hexa_char)){
			setlocale(LC_CTYPE, "");
			*ptrint  =  hexa_char - '0';
			return SUCCEEDED;
		}else{
			setlocale(LC_CTYPE, "");
			*ptrint = tolower(hexa_char) - 'a' + 10 ;
			return SUCCEEDED;
		}
	}else{
		setlocale(LC_CTYPE, "");
		return ERR_OUTRAG;
	}
}

/*!	\fn	char int2hexachar(int num)
 * 		\brief	This function convert an integer number to its correspondent hexa character
 * 		\param		num	the integer to convert
 *		\param		ptrch	the pointer that contains the address of the output char.
 * 		\return		SUCCEEDED if having no error \n
 * 						error number if having an error
 */
int	int2hexachar(int num, char* ptrch){
	if( num < 0 || num >15){
		return ERR_OUTRAG;
	}else{
		if(num <10)
			*ptrch = (char) ('0' + num);
		else
			*ptrch = (char) ('A' + num - 10);
		return SUCCEEDED;
	}
}

/*!	\fn	int	hexachars2bytes(Bytes* hexa_chars, Bytes* binary_bytes)
 * 		\brief	This function convert a hexa character array to its correspondent byte array
 * 		\param		hexa_chars		a ::Bytes struct that holds the hexa character array to convert \n
 * 													 Its length member contains the number of hexa character in the input array.
 * 		\param		binary_bytes	a ::Bytes struct that holds the result binary array. \n
 * 													 Its length member contains the number of bits in the result array.
 * 		\return		SUCCEEDED if having no error \n
 * 						error number if having an error
 */
int	hexachars2bytes(bytes* hexa_chars, bytes* binary_bytes){
	int i, tmp, byte_len, err =0;

	byte_len = hexa_chars->length/2 +1;
	binary_bytes->bytes = (char*) calloc(byte_len, sizeof(char));

	if(!binary_bytes->bytes){
		return ERR_OUTMEM;
	}

	for(i=0; i< byte_len; i++){

		err = hexachar2int(hexa_chars->bytes[2*i], &tmp);
		if(err != SUCCEEDED) return err;

		if( 2*i <= hexa_chars->length - 2){
				binary_bytes->bytes[i] = tmp << 4;
				err = hexachar2int(hexa_chars->bytes[2*i+1], &tmp);
				if(err != SUCCEEDED) return err;
				binary_bytes->bytes[i] |= tmp;
				if( 2*i == hexa_chars->length-2){
					binary_bytes->length = 8*(i+1);
					break;
				}
		}else{
			binary_bytes->bytes[i] = tmp << 4;
			binary_bytes->length = 8*i +4 ;
			break;
		}
	}
	return SUCCEEDED;

}

/*!	\fn		int bytes2hexachars(Bytes* binary_bytes, Bytes* hexa_chars)
 * 		\brief	This function converts a bytes character array to its conrrespondent hexa character array
 *		\param	binary_bytes: a ::Bytes struct that holds the binary array to convert. \n
 * 											 Its length member contains the number of bits in the result array.
 * 		\param hexa_chars:  a ::Bytes struct that holds the output hexadecimal array. \n
 * 											 Its length member contains the number of characters in the result array.
 *
 * 		\return	SUCCEEDED(0) if successfully converted \n
 * 					error number if having an error
 */
int bytes2hexachars(bytes* binary_bytes, bytes* hexa_chars){
		int i, tmp, err = 0 ;
		hexa_chars->length = binary_bytes->length / 4;
		hexa_chars->bytes = (char*) calloc(hexa_chars->length, sizeof(char));
		if( !hexa_chars->bytes)
			return ERR_OUTMEM;

		for( i = 0; i < hexa_chars->length; i+=2){
			if(i+1 < hexa_chars->length ){
				tmp = (binary_bytes->bytes[i/2] & 0xF0) >> 4;
				if( ( err = int2hexachar(tmp, &hexa_chars->bytes[i])) != SUCCEEDED) return err;
				tmp = binary_bytes->bytes[i/2] & 0x0F;
			 	if( (err = int2hexachar(tmp, &hexa_chars->bytes[i+1])) != SUCCEEDED) return err;
			}else{
				tmp = (binary_bytes->bytes[i/2+1] & 0xF0) >> 4;
				if( (err = int2hexachar(tmp, &hexa_chars->bytes[i])) != SUCCEEDED) return err;
				break;
			}
		}
		return SUCCEEDED;
}

 /*!		\fn		int verify_data(bytes*)
 * 			\brief	This function check whether a bytes struct has data or not
 * 			\param	 ptrbytes a bytes struct pointer that will be verified
 * 			\return  HASDATA (0) if ptrbytes->bytes != NULL and ptrbytes->length > 0 \n
 * 						LENZERO	 (1) if ptrbytes->bytes != NULL and ptrbytes ->length <=0 \n
 * 						DATNULL	 (2) if ptrbytes->bytes == NULL and ptrbytes->length !=0 \n
 * 						EMTDATA (3) in ptrbytes->bytes == NULL and ptrbytes->length ==0 \n
 */
 int verify_bytes(bytes* ptrbytes){
 	if(ptrbytes->bytes != NULL){
 		if(ptrbytes->length > 0)
 			return HASDATA;
 		else
 			return LENZERO;
 	}else{
 		if(ptrbytes->length != 0)
 			return DATNULL;
  		else
 			return EMTDATA;
 	}
 }

/*!		\fn		void dump_bytes(bytes*)
 * 			\brief	This function display a byte array in a special format */
 void dump_bytes(bytes* ptrbytes){
 	int i =0;
 	if(verify_bytes(ptrbytes) != HASDATA){
 		printf("The bytes struct has not contained data \n");
 		return;
 	}
 	setlocale(LC_CTYPE, "C");
 	printf("Each char is seperated with a space.\n If a char is a space, it will be displayed as SP. \n \
 	If a chacter is not printable, it will be display in hexadecimal format. \n");
 	for(i =0 ; i < ptrbytes->length; i++){
 		char ch = *(ptrbytes->bytes +i);
 		/* Is this char printable? */
 		if( isprint(ch)){
			if( isspace(ch))
				/* Is this char is a space character */
				printf(" SP");
			else
				printf(" %c", ch);
 		}else{
 		/* This char is not printable, it will be printed in hexadecimal format */
 			printf(" 0x%x", ch);
 		}
 	}
 	setlocale(LC_CTYPE, "");
 }

 /*!		\fn		int verify_datatype(bytes*, int)
 * 			\brief	This function checks whether a bytes struct has its data conformed to a specified datatype
 * 			\param	 ptrbytes a bytes struct pointer that will be verified
 * 			\return   CONFORM(0) if the struct's data conform to the specified datatype
 * 						 NOT_CONFORM if the struct's data doesn't conform to the specified datatype
 */
 int verify_datatype(bytes* ptrbytes, int datatype){
 	int i=0;

 	/* verify the data of the struct	*/
	if(verify_bytes(ptrbytes) != HASDATA)
		return NOT_CONFORM;
	switch(datatype){
		case ISO_BINARY:{
			return CONFORM;
		}
		break;
		case ISO_ALPHABETIC:{			// Alphabetic
				while(i < ptrbytes->length){
					if(is_in_range(letter_range, *(ptrbytes->bytes + i)) != IN_RANGE)
						return NOT_CONFORM;
					i++;
				}
				return CONFORM;
		}
		break;
		case ISO_NUMERIC:{			// Numeric
				while(i < ptrbytes->length){
					if(is_in_range(numeric_range, *(ptrbytes->bytes + i)) != IN_RANGE)
							return NOT_CONFORM;
					i++;
				}
				return CONFORM;
		}
		break;
		case ISO_Z:{			// Z
			return CONFORM;
		}
		break;
		case ISO_ALPHANUMERIC:{		// AN datatype
			while(i < ptrbytes->length){
				if(is_in_range(numeric_range, *(ptrbytes->bytes + i)) != IN_RANGE && is_in_range(letter_range, *(ptrbytes->bytes + i)) != IN_RANGE)
						return NOT_CONFORM;
				i++;
			}
			return CONFORM;
		}
		break;
		case ISO_ALPHASPECIAL:{		//	AS datatype
			while(i < ptrbytes->length){
				if(is_in_range(letter_range, *(ptrbytes->bytes + i)) != IN_RANGE && is_in_range(special_range, *(ptrbytes->bytes + i)) != IN_RANGE)
						return NOT_CONFORM;
				i++;
			}
			return CONFORM;
		}
		break;
		case ISO_NUMERICSPECIAL:{	//	NS datatype
			while(i < ptrbytes->length){
				if(is_in_range(numeric_range, *(ptrbytes->bytes + i)) != IN_RANGE && is_in_range(special_range, *(ptrbytes->bytes + i)) != IN_RANGE)
						return NOT_CONFORM;
				i++;
			}
			return CONFORM;
		}
		break;
		case ISO_ALPHANUMERIC_PAD:{		// ANP datatype
			while(i < ptrbytes->length){
				if(is_in_range(numeric_range, *(ptrbytes->bytes + i)) != IN_RANGE && is_in_range(letter_range, *(ptrbytes->bytes + i)) != IN_RANGE \
				&& is_in_range(pad_range, *(ptrbytes->bytes + i)) != IN_RANGE)
						return NOT_CONFORM;
				i++;
			}
			return CONFORM;
		}
		break;
		case ISO_ALPHANUMERIC_SPC:{		// ANS datatype
			while(i < ptrbytes->length){
				if(is_in_range(numeric_range, *(ptrbytes->bytes + i)) != IN_RANGE && is_in_range(numeric_range, *(ptrbytes->bytes + i)) != IN_RANGE \
				&& is_in_range(special_range, *(ptrbytes->bytes + i)) != IN_RANGE)
						return NOT_CONFORM;
				i++;
			}
			return CONFORM;
		}
		break;
		default:
		break;
	}
	return CONFORM;
 }


 /*!		\fn		void empty_bytes(bytes*);
 * 			\brief	This function make a bytes struct empty that is its bytes = NULL and its length = 0
 * 			\param  ptrbytes a bytes struct pointer that will be made empty
 */
 void empty_bytes(bytes* ptrbytes){
 	ptrbytes->bytes = NULL;
 	ptrbytes->length = 0;
 }

/*!		\fn 			copy_data(bytes*, char*)
 * 			\brief		This function copies data to a bytes struct
 * 			\param		ptrbytes a bytes struct pointer that will be set data
 * 			\param		the data buffer that will be copied to ptrbytes
 * 			\param		the length of ptrchar
 * 			\return		SUCCEEDED (0) if successfully copied data
 * 							error number in case having an error
 */
 int import_data(bytes* ptrbytes, const char* ptrchar, int len){
 		ptrbytes->length = len;
 		ptrbytes->bytes = (char*)calloc(ptrbytes->length, sizeof(char));
 		if(ptrbytes->bytes != NULL){
			memcpy(ptrbytes->bytes, ptrchar, len);
			return SUCCEEDED;
 		}else{
 			ptrbytes->length = 0;
 			return ERR_OUTMEM;
 		}
 }

 /*!		\fn 	export_data(bytes*, char*, int*)
 * 			\brief	This function copy data from a bytes struct to a buffer
 * 			\param		ptrbytes a bytes struct pointer whose data will be copied
 * 			\param		ptrchar the data buffer to which the data will be copied
 * 			\param		ptrlen 	the length of the copied data will be set to this integer pointer
 * 			\return		SUCCEEDED (0) if successfully copied \n
 * 							error number in case having an error
 */
int export_data(bytes* ptrbytes, char** ptrchar, int* ptrlen){
 		*ptrlen = ptrbytes->length;
 		if(ptrchar != NULL && ptrbytes->bytes != NULL){
 			*ptrchar = (char*) calloc(*ptrlen + 1, sizeof(char));
 			if(*ptrchar == NULL)
 				return ERR_OUTMEM;
 			memcpy(*ptrchar, ptrbytes->bytes, *ptrlen);
 			return SUCCEEDED;
 		}else{
 			return ERR_CPYNUL;
 		}
 }

/*!	\func		int append_bytes(bytes* ptrdes, bytes*ptrsrc)
 * 		\brief		This function appends data of a bytes struct to another bytes struct.
 * 		\param		ptrdes is a bytes struct pointer of the destination bytes
 * 		\param		ptrsrc is a bytes struct pointer of the source bytes
 * 		\return		SUCCEEDED (0) if successfully copied \n
 * 							error number in case having an error
 */
int append_bytes(bytes* ptrdes, bytes*ptrsrc){
	char* tmp;
	int err = 0;
	int len = ptrdes->length + ptrsrc->length;
	if( verify_bytes(ptrsrc) != HASDATA){
		return ERR_APDNUL;
	}
	tmp = (char*) calloc( len , sizeof(char));
	if(tmp == NULL)
		return ERR_OUTMEM;
	memcpy(tmp, ptrdes->bytes, ptrdes->length);
	memcpy(tmp + ptrdes->length, ptrsrc->bytes, ptrsrc->length);
	free_bytes(ptrdes);
	err = import_data(ptrdes, tmp, len);
	free(tmp);
	return err;
}

/*!	\func
 * 		\brief	This function insert data of a bytes struct to a specified position of data of another bytes struct
 * 		\param		ptrdes is a bytes struct pointer of the destination bytes
 * 		\param		ptrsrc is a bytes struct pointer of the source bytes
 * 		\param		pos is the position in the data of ptrdes at which data of ptrsrc will be inserted
 * 		\return		SUCCEEDED (0) if successfully copied \n
 * 							error number in case having an error
 */
int insert_bytes(bytes* ptrdes, bytes* ptrsrc, int pos){
	char* tmp;
	int err=0, len;
	/* verify ptrsrc data */
	if( verify_bytes(ptrsrc) != HASDATA)
		return ERR_INSNUL;
	if( pos < 0 || pos > ptrdes->length+1)
		return ERR_IVLPOS;

	len = ptrdes->length + ptrsrc->length;
	tmp = (char*) calloc(len, sizeof(char));
	if(tmp == NULL)
		return ERR_OUTMEM;
	if( pos == 0){
		memcpy(tmp, ptrsrc->bytes, ptrsrc->length);
		memcpy(tmp + ptrsrc->length, ptrdes->bytes, ptrdes->length);
	}else{
		memcpy(tmp, ptrdes->bytes, pos-1);
		memcpy(tmp + pos-1, ptrsrc->bytes, ptrsrc->length);
		memcpy(tmp + pos -1 + ptrsrc->length, ptrdes->bytes+ pos -1, len - pos - ptrsrc->length +1);
	}
	free_bytes(ptrdes);
	err = import_data(ptrdes, tmp, len);
	free(tmp);
	return err;
}

 /*!		\fn 	set_length(bytes*, int)
 * 			\brief	This function sets length for a bytes struct
 * 			\param		ptrbytes a bytes struct pointer that will be set length
 * 			\param		len the length that will be set as the length of ptrbytes
 */
 void set_length(bytes* ptrbytes, int len){
		ptrbytes->length = len;
 }

 /*!	\fn int get_length(bytes*)
 * 		\brief	This function gets length of a bytes struct
 * 		\param		ptrbytes a bytes struct pointer
 * 		\return		ptrbytes->length;
 */
 int get_length(bytes* ptrbytes){
		return ptrbytes->length;
 }

/*!		\fn		void free_bytes(bytes*);
 * 			\brief	This function frees a bytes struct then makes it empty that is its bytes = NULL and its length = 0
 * 			\param  ptrbytes a bytes struct pointer that will be made empty
 */
 void free_bytes(bytes* ptrbytes){
 		if(ptrbytes->bytes != NULL) free(ptrbytes->bytes);
 		empty_bytes(ptrbytes);
 }

int	int2bin(unsigned int in){
	int i =0 ;
	char out[9];
	do{
		if(in & 0x80)
			out[i] = '1';
		else
			out[i] = '0';
		in <<= 1;
	    i++;
	}while( i < 8);
	out[8] = '\0';
	printf("\n %s \n", out);
	return 0;
}

/*!	\fn			int left_pad(bytes*, char);
 * 		\brief 		This function pads the left side of a byte array with a character
 * 		\param		ptrbytes	a pointer to a ::bytes struct to be padded
 * 		\param		max_len	the maximum length after padding
 * 		\param		ch	a charater that will be used to pad with
 * 		\return		SUCCEEDED  if successfully padding \n
 * 						error number if having an error
 */
int left_pad(bytes* ptrbytes, int max_len, char ch){
	int i = 0, err = 0 ;
	char* tmp_bytes;

	/* verify the bytes struct */
	err = verify_bytes(ptrbytes) ;
	if(err != HASDATA)
		return err;

	if( ptrbytes->length > max_len)
		return ERR_OVRLEN;
	tmp_bytes = (char*) calloc( max_len, sizeof(char));
	if(tmp_bytes == NULL){
		return ERR_OUTMEM;
	}
	memcpy(tmp_bytes + (max_len - ptrbytes->length), ptrbytes->bytes, ptrbytes->length);
	for(i = 0; i < (max_len - ptrbytes->length); i ++){
		*(tmp_bytes + i) = ch;
	}
	/* free the old bytes struct */
	free_bytes(ptrbytes);

	/* create a new bytes struct */
	err = import_data(ptrbytes, tmp_bytes, max_len);
	if(err != SUCCEEDED){
		free(tmp_bytes);
		return err;
	}
	return SUCCEEDED;
}

/*!	\fn		int right_pad(bytes*, char);
 * 		\brief 	This function pads the right side of a byte array with a character
 * 		\param		ptrbytes	a pointer to a ::bytes struct to be padded
 * 		\param		max_len	the maximum length after padding
 * 		\param		ch	a charater that will be used to pad with
 * 		\return		SUCCEEDED if successfully padding \n
 * 						error number if having an error
 */
int right_pad(bytes* ptrbytes, int max_len, char ch){
	int i = 0, err = 0 ;
	char* tmp_bytes;

	/* verify the bytes struct */
	err = verify_bytes(ptrbytes) ;
	if(err != HASDATA)
		return err;

	if( ptrbytes->length > max_len)
		return ERR_OVRLEN;
	tmp_bytes = (char*) calloc( max_len, sizeof(char));
	if(tmp_bytes == NULL){
		return ERR_OUTMEM;
	}
	memcpy(tmp_bytes, ptrbytes->bytes, ptrbytes->length);
	for(i = ptrbytes->length; i < max_len; i ++){
		*(tmp_bytes + i) = ch;
	}
	/* free the old bytes struct */
	free_bytes(ptrbytes);

	/* create a new bytes struct */
	err = import_data(ptrbytes, tmp_bytes, max_len);
	if(err != SUCCEEDED){
		free(tmp_bytes);
		return err;
	}
	return SUCCEEDED;
}

/*!	\fn			int left_trim(bytes*, char);
 * 		\brief 		This function trims the left side of a byte array
 * 		\param		ptrbytes	a pointer to a ::bytes struct to be trimmed
 * 		\param		ch	a charater that will be trimmed out
 * 		\return		SUCCEEDED if successfully trimming \n
 * 						error number			 if having an error
 */
int left_trim(bytes* ptrbytes, char ch){
	int i = 0, err = 0 ;
	bytes tmp_bytes;
	err = verify_bytes(ptrbytes) ;
	if(err != HASDATA)
		return err;
	/* copy data to a temporary struct */
	empty_bytes(&tmp_bytes);
	err = import_data(&tmp_bytes, ptrbytes->bytes, ptrbytes->length);
	if(err != SUCCEEDED)
		return err;
	free_bytes(ptrbytes);
	/* process the temporary struct */
	while(*(tmp_bytes.bytes + i) == ch && i < tmp_bytes.length){
		i++;
	}
	if(i < tmp_bytes.length){
		err = import_data(ptrbytes, tmp_bytes.bytes + i, tmp_bytes.length - i);
		free_bytes(&tmp_bytes);
		setlocale(LC_CTYPE, "");
		return err;
	}
	setlocale(LC_CTYPE, "");
	free_bytes(&tmp_bytes);
	return SUCCEEDED;
}

/*!	\fn		int right_trim(bytes*, char);
 * 		\brief 	This function trims the right side of a byte array
 * 		\param		ptrbytes	a pointer to a ::bytes struct to be trimmed
 * 		\param		ch	a charater that will be trimmed out
 * 		\return		SUCCEEDED if successfully trimming \n
 * 						error number			 if having an error
 */
int right_trim(bytes* ptrbytes, char ch){
	int i = ptrbytes->length -1, err = 0 ;
	bytes tmp_bytes;
	err = verify_bytes(ptrbytes) ;
	if(err != HASDATA)
		return err;
	/* copy data to a temporary struct */
	empty_bytes(&tmp_bytes);
	err = import_data(&tmp_bytes, ptrbytes->bytes, ptrbytes->length);
	if(err != SUCCEEDED)
		return err;
	free_bytes(ptrbytes);
	/* process the temporary struct */
	while(*(tmp_bytes.bytes + i) == ch && i >=0 ){
		i--;
	}
	if(i >= 0){
		err = import_data(ptrbytes, tmp_bytes.bytes, i+1);
		free_bytes(&tmp_bytes);
		return err;
	}
	free_bytes(&tmp_bytes);
	return SUCCEEDED;
}

/*!	\fn		int is_in_range(int** , char);
 * 		\brief 		check whether a char is in a character range
 * 		\param		cr	a range to check
 * 		\param		ch the character that will be checked
 * 		\return		INRANGE(0) if the char is in the range	\n
 * 						NOTINRANGE(1) if the char is not in the range
 */
int is_in_range(char_range* cr, char ch){
	while(cr->max != -1){
		if( ch >= cr->min && ch <= cr->max){
			return IN_RANGE;
		}else{
			cr ++;
		}
	}
	return NOT_IN_RANGE;
}
