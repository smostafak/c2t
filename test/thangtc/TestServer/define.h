#ifndef DEFINE_H_
#define DEFINE_H_
#include <stdio.h>

typedef struct 
{
	int index;
	char *func_name;
} arrFunc;
static const arrFunc Func_Arr[] = {
	{0, "test1"},
	{1, "test2"}
};
int test1();
int test2();
//int (*p[2])() = {test1, test2};
#endif /*DEFINE_H_*/
