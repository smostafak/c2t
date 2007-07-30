/*!	\file		utilities.h
 * 		\brief	This header file contains all structures and utility function prototypes
 */
#ifndef UTILITIES_H_
#define UTILITIES_H_

/*!	\struct	bytearray
 * 		\brief	a structure that holds an array of bytes and its length.
 */ 
struct  bytearray{
	int length;		
	char* bytes;
};

/*!	\brief	Bytes is the alias of "struct bytearray" */
typedef struct  bytearray	Bytes;

/*		\fn 		int 	hexachar2int(char );
 * 		\brief	This function converts a hexa character to its correspondent integer value 	
 */
int 	hexachar2int(char );

/*		\fn		char*	hexachars2bytes(char*);
 * 		\brief	This function converts a hexa character array to its correspondent byte array
 */
int	hexachars2bytes(Bytes* , Bytes* );

/*!	\fn	char int2hexachar(int num)	
 * 		\brief	This function convert an integer number to its correspondent hexa character
 */
char	int2hexachar(int );

/*		\fn		char* bytes2hexachars(char*);
 * 		\brief	This function converts a bytes character array to its conrrespondent hexa character array
 */
int bytes2hexachars(Bytes*, Bytes*);
 
int	int2bin(unsigned int);

/*!	\fn	char *lpad(char *s, int len, char ch);		 
 * 		\brief	
 * 		\param	s is a pointer to a string
 * 		\param  len is number 
 * 		\param  ch is character fill    
 */
char *lpad(char *s, int len, char ch);

/*!	\fn	char *rpad(char *s, int len, char ch);		 
 * 		\brief	
 * 		\param	s is a pointer to a string
 * 		\param  len is number 
 * 		\param  ch is character fill    
 */
char *rpad(char *s, int len, char ch);	

/*!	\fn	int is_numeric(char *s)		 
 * 		\brief return 1 if string s is numeric string, return 0 if otherwise
 * 		\param	s is a pointer to a string     
 */
int is_numeric(char *s);

#endif /*UTILITIES_H_*/
