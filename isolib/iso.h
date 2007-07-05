#ifndef ISO_H
#define ISO_H

#include <stdio.h>

#define ISO_BITMAP       0
#define ISO_NUMERIC      1
#define ISO_ALPHANUMERIC 2
#define ISO_BINARY       3

typedef struct {
	int format;
	int lenflds;
	int flds;
	const char *dsc;
} isodef;

typedef void *isomsg;
typedef unsigned char byte;

void init_msg(isomsg *msg);
int pack(const isomsg *msg, const isodef *def, char *buf);
int unpack(isomsg *msg, const isodef *def, const char *buf);
int dump(FILE *fp, const isomsg *msg);
void free_msg(isomsg *msg);

int getstring(const isomsg *msg, const isodef *def, byte idx, char *val);
int getint(const isomsg *msg, const isodef *def, byte idx, int *val);
int getlong(const isomsg *msg, const isodef *def, byte idx, long *val);
int getdouble(const isomsg *msg, const isodef *def, byte idx, double *val);

int setstring(isomsg *msg, const isodef *def, byte idx, const char *val);
int setint(isomsg *msg, const isodef *def, byte idx, int val);
int setlong(isomsg *msg, const isodef *def, byte idx, long val);
int setdouble(isomsg *msg, const isodef *def, byte idx, double val);

int getstring(const char *buff, const isodef *def, byte idx, char *val);
int getint(const char *buff, const isodef *def, byte idx, int *val);
int getlong(const char *buff, const isodef *def, byte idx, long *val);
int getdouble(const char *buff, const isodef *def, byte idx, double *val);

#endif