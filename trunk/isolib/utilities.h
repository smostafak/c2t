/*!	\file		utilities.h
 * 		\brief	This header file contains all structures and utility function prototypes
 */
#ifndef UTILITIES_H_
#define UTILITIES_H_

#define  HASDATA	0		/* contain data */
#define  LENZERO	1		/* length is zero */
#define 	DATNULL	2		/* data is null */
#define  EMTDATA 3		/*	both data and length empty */

#define  SUCCEEDED 			0
#define  FAILED			 			1
#define  CONFORM				0
#define	NOT_CONFORM 		1
#define  IN_RANGE				0
#define	NOT_IN_RANGE		1
#define  SPACE						31


/*!	\struct	range
 * 		\brief	a structure that holds a minimum and a maximum value of a character range
 */
typedef struct{
	int min;
	int max;
}char_range;

/*!	\brief	the code ranges of special charaters */
static char_range		special_range[]={{33,47}, {58,64}, {91,96}, {123,126}, {-1, -1}};

/*!	\brief	the code ranges of letter charaters */
static char_range		letter_range[]= {{65,90}, 	{97,122}, {-1, -1}};

/*!	\brief	the code ranges of numeric charaters */
static char_range		numeric_range[]={{48, 57}, {-1, -1}};

/*!	\brief	the code ranges of pad charaters */
static char_range		pad_range[]={{32, 32}, {-1, -1}};



/*!	\struct	bytearray
 * 		\brief	a structure that holds an array of bytes and its length.
 */
struct  bytearray{
	int length;
	char* bytes;
};

/*!	\brief	Bytes is the alias of "struct bytearray" */
typedef struct  bytearray	bytes;

/*! 		\brief	This function converts a hexa character to its correspondent integer value
 */
int 	hexachar2int(char );

/*!	\brief	This function converts a hexa character array to its correspondent byte array */
int	hexachars2bytes(bytes* , bytes* );

/*!	\brief	This function convert an integer number to its correspondent hexa character */
char	int2hexachar(int );

/*!	\brief	This function converts a bytes character array to its conrrespondent hexa character array */
int bytes2hexachars(bytes*, bytes*);

/*!	\brief	This function makes a bytes struct empty that is its bytes = NULL and its length = 0 */
 void empty_bytes(bytes*);

/*!	\brief	This function checks whether a bytes struct has data or not */
 int verify_bytes(bytes*);

/*!	\brief	This function display a byte array in a special format */
 void dump_bytes(bytes*);

/*!	\brief	This function checks whether a bytes struct has its data conformed to a specified datatype */
 int verify_datatype(bytes*, int);

 /*!	\brief	This function frees a bytes struct then makes it empty that is its bytes = NULL and its length = 0  */
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

/*!	\fn		int left_pad(bytes*, char);
 * 		\brief 	This function pads the left side of a byte array with a character
 */
int left_pad(bytes*, int, char);

/*!	\fn		int right_pad(bytes*, char);
 * 		\brief 	This function pads the right side of a byte array with a character
 */
int right_pad(bytes*, int, char);

/*!	\fn		int left_trim(bytes*, char);
 * 		\brief 	This function trims the left side of a byte array
 */
int left_trim(bytes*, char);

/*!	\fn		int right_trim(bytes*, char);
 * 		\brief 	This function trims the right side of a byte array
 */
int right_trim(bytes*, char);

/*!	\fn	int is_in_range(int** , char);
 * 		\brief 	check whether a char is in a character range
 */
int is_in_range(char_range* , char);

#endif /*UTILITIES_H_*/
