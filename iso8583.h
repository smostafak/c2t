#ifndef _ISO8583_H
#define _ISO8583_H 1

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

typedef struct {
	char *fld[129];
} isomsg;

void iso8583_init(isomsg *m);
int iso8583_pack(const isomsg *m, const isodef *d, char *buf);
int iso8583_unpack(isomsg *m, const isodef *d, const char *buf);
void iso8583_dump(FILE *fp, isomsg *m);
void iso8583_free(isomsg *m);

#endif /* iso8583.h */
