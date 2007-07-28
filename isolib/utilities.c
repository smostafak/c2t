/*!	\file		utilities.c
 * 		\brief	This file contains all functions that support main iso-related functions
 */
#include "utilities.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*!	\fn	int hexachar2int(char hexa_char)
 * 		\brief	This function convert a hexa character to its correspondent integer value
 * 		\param		hexa_char	the character to convert
 *
 * 		\return		the correspondent integer value of this hexa_char
 * 						or  -1 if having errors.
 */
int	hexachar2int(char hexa_char){
	if( isxdigit(hexa_char)){
		if( isdigit(hexa_char))
			return hexa_char - '0';
		else
			return tolower(hexa_char) - 'a' + 10 ;
	}else{
		return -1;
	}

}

/*!	\fn	char int2hexachar(int num)
 * 		\brief	This function convert an integer number to its correspondent hexa character
 * 		\param		num	the integer to convert
 *
 * 		\return		the correspondent hexa character of the integer number
 * 						or  -1 if having errors
 */
char	int2hexachar(int num){
	if( num < 0 || num >15){
		return -1;
	}else{
		if(num <10)
			return (char) ('0' + num);
		else
			return (char) ('A' + num - 10);
	}
}

/*!	\fn	int	hexachars2bytes(Bytes* hexa_chars, Bytes* binary_bytes)
 * 		\brief	This function convert a hexa character array to its correspondent byte array
 * 		\param		hexa_chars		a ::Bytes struct that holds the hexa character array to convert \n
 * 													 Its length member contains the number of hexa character in the input array.
 * 		\param		binary_bytes	a ::Bytes struct that holds the result binary array. \n
 * 													 Its length member contains the number of bits in the result array.
 *
 *
 * 		\return		the number of bytes of the result array
 * 						-1 if having an error.
 */
int	hexachars2bytes(Bytes* hexa_chars, Bytes* binary_bytes){
	int i, tmp, byte_len;

	byte_len = hexa_chars->length/2 +1;
	binary_bytes->bytes = (char*) calloc(byte_len, sizeof(char));

	if(!binary_bytes->bytes){
		return -2;
	}

	for(i=0; i< byte_len; i++){

		tmp = hexachar2int(hexa_chars->bytes[2*i]);
		if(tmp < 0) return -1;

		if( 2*i <= hexa_chars->length - 2){
				binary_bytes->bytes[i] = tmp << 4;
				tmp = hexachar2int(hexa_chars->bytes[2*i+1]);
				if(tmp < 0) return -1;
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
	return i+1;

}

/*!	\fn		int bytes2hexachars(Bytes* binary_bytes, Bytes* hexa_chars)
 * 		\brief	This function converts a bytes character array to its conrrespondent hexa character array
 *		\param	binary_bytes: a ::Bytes struct that holds the binary array to convert. \n
 * 											 Its length member contains the number of bits in the result array.
 * 		\param hexa_chars:  a ::Bytes struct that holds the output hexadecimal array. \n
 * 											 Its length member contains the number of characters in the result array.
 *
 * 		\return	the pointer of the correspondent hexa character array of this hexa byte array
 * 					NULL if having error
 */
int bytes2hexachars(Bytes* binary_bytes, Bytes* hexa_chars){
		int i, tmp;
		hexa_chars->length = binary_bytes->length / 4;
		hexa_chars->bytes = (char*) calloc(hexa_chars->length, sizeof(char));
		if( !hexa_chars->bytes)
			return -1;

		for( i = 0; i < hexa_chars->length; i+=2){
			if(i+1 < hexa_chars->length ){
				tmp = (binary_bytes->bytes[i/2] & 0xF0) >> 4;
				if( (hexa_chars->bytes[i] = int2hexachar(tmp)) < 0) return -1;
				tmp = binary_bytes->bytes[i/2] & 0x0F;
			 	if( (hexa_chars->bytes[i+1] = int2hexachar(tmp)) < 0) return -1;
			}else{
				tmp = (binary_bytes->bytes[i/2+1] & 0xF0) >> 4;
				if( (hexa_chars->bytes[i] = int2hexachar(tmp)) < 0) return -1;
				break;
			}
		}
		return 0;
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

/*!	\fn	char *lpad(char *s, int len, char ch);
 * 		\brief
 * 		\param	s is a pointer to a string
 * 		\param  len is number
 * 		\param  ch is character fill
 */
char *lpad(char *s, int len, char ch)
{
    int i = strlen(s);

    if (i >= len) return(s);
    memmove(s+len-i,s,(size_t)(i+1));
    memset(s,ch,(size_t)(len-i));
    return s;
}

/*!	\fn	char *rpad(char *s, int len, char ch);
 * 		\brief
 * 		\param	s is a pointer to a string
 * 		\param  len is number
 * 		\param  ch is character fill
 */
char *rpad(char *s, int len, char ch)
{
    int i = strlen(s);

    if (i >= len) return(s);
    memset(s+i, ch, (size_t)(len - i));
    return s;
}
