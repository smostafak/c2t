/*!	\file		utilities.h
 * 		\brief	This header file contains all structures and utility function prototypes
 */
#ifndef UTILITIES_H_
#define UTILITIES_H_

#define  HASDATA	0		/* contain data */
#define  LENZERO	1		/* length is zero */
#define 	DATNULL	2		/* data is null */
#define  EMTDATA 3		/*	both data and length empty */

#define  SUCCEEDED 	0
#define  FAILED			 	1
/*!	\struct	bytearray
 * 		\brief	a structure that holds an array of bytes and its length.
 */
struct  bytearray{
	int length;
	char* bytes;
};

/*!	\brief	Bytes is the alias of "struct bytearray" */
typedef struct  bytearray	bytes;

/*		\fn 		int 	hexachar2int(char );
 * 		\brief	This function converts a hexa character to its correspondent integer value
 */
int 	hexachar2int(char );

/*		\fn		char*	hexachars2bytes(char*);
 * 		\brief	This function converts a hexa character array to its correspondent byte array
 */
int	hexachars2bytes(bytes* , bytes* );

/*!	\fn	char int2hexachar(int num)
 * 		\brief	This function convert an integer number to its correspondent hexa character
 */
char	int2hexachar(int );

/*!		\fn		char* bytes2hexachars(char*);
 * 		\brief	This function converts a bytes character array to its conrrespondent hexa character array
 */
int bytes2hexachars(bytes*, bytes*);

/*!		\fn		void empty_bytes(bytes*);
 * 			\brief	This function makes a bytes struct empty that is its bytes = NULL and its length = 0
 */
 void empty_bytes(bytes*);

/*!		\fn		int verify_bytes(bytes*)
 * 			\brief	This function checks whether a bytes struct has data or not
 */
 int verify_bytes(bytes*);

 /*!		\fn		void free_bytes(bytes*);
 * 			\brief	This function frees a bytes struct then makes it empty that is its bytes = NULL and its length = 0
 */
 void free_bytes(bytes*);

/*!		\fn 	set_data(bytes*, char*)
 * 			\brief	This function copy data to a bytes struct
 */
 int import_data(bytes*, const char*, int);

/*!		\fn 	export_data(bytes*, char*, int*)
 * 			\brief	This function copy data from a bytes struct to a buffer
 */
 int export_data(bytes*, char*, int*);

 /*!		\fn 	set_length(bytes*, int)
 * 			\brief	This function sets length for a bytes struct
 */
 void set_length(bytes*, int);

/*!	\fn int get_length(bytes*)
 * 		\brief	This function gets length of a bytes struct
 */
 int get_length(bytes*);


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

#endif /*UTILITIES_H_*/
