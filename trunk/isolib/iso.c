#include <string.h>
#include "iso.h"

typedef struct {
	char *fld[129];
} isomsg_t;


void init_msg(isomsg *msg) {
	isomsg_t *pmsg = (isomsg_t *) malloc(sizeof(isomsg_t));	
	
	int i;
	for (i = 0; i < 129; i++) {
		pmsg->fld[i] = NULL;
	}
	*msg = pmsg;
}

int pack(const isomsg *msg, const isodef *def, char *buf) {
	isomsg_t *pmsg = (isomsg_t *) *msg;
	char *start = buf;
	int flds;
	int i;
	char *bitmap;
	int len;
	char tmp[20];

	/* Field 0 is mandatory and fixed length. */
	if (strlen(pmsg->fld[0]) != def[0].flds || def[0].lenflds != 0) {
		/* FIXME: error */
	}
	memcpy(buf, pmsg->fld[0], def[0].flds);
	buf += def[0].flds;

	/*
	 * The bitmap contains either 64 or 128 fields - flds is the 
	 * number of allowed fields, i.e. either 64 or 128.
	 */
	flds = 64;
	for (i = 65; i <= 128; i++) {
		if (pmsg->fld[i] != NULL) {
			flds = 128;
			break;
		}
	}

	bitmap = (char *) calloc(flds/8 + 1, sizeof(char));
	/*
	 * First bit in the bitmap (field 1) defines if the message is 
	 * extended or not.
	 * 0: not exteded - i.e. only (some of) the fields 0 to 64 are 
	 *    used
	 * 1: extended - i.e. (some of) the fields 0 to 128 are used
	 * I.e. if one or more of the fields 65 to 128 is used, the 
	 * bit is 1.
	 */
	if (flds == 128) {
		bitmap[0] |= 0x80;
	}

	/*
	 * The bits 2 to 64/128 defines if the corresponding field is 
	 * present (1) or not (0).
	 */
	for (i = 2; i <= flds; i++) {
		if (pmsg->fld[i] != NULL) {
			/* Set the i'th bit to 1 */
			bitmap[(i-1)/8] |= 0x80 >> ((i-1)%8);
		}
	}
	memcpy(buf, bitmap, flds/8);
	buf += flds/8;

	for (i = 2; i <= flds; i++) {
		if ((bitmap[(i-1)/8] << ((i-1)%8)) & 0x80) { /* i'th bit != 0 */
			if (def[i].lenflds) { /* Variable length */
				switch (def[i].format) {
				case ISO_NUMERIC: /* Fallthrough */
				case ISO_ALPHANUMERIC:
					len = strlen(pmsg->fld[i]);
					sprintf(tmp, "%%0%dd", def[i].lenflds);
					sprintf(buf, tmp, len);
					if (len > def[i].flds) {
						/* FIXME: error */
					}
					buf += def[i].lenflds;
					break;
				case ISO_BINARY:
					sprintf(tmp, "%%0%dd", def[i].lenflds);
					sscanf(pmsg->fld[i], tmp, &len);
					if (len > def[i].flds) {
						/* FIXME: error */
					}
					/* Copy length bytes from pmsg->fld[i] */
					len += def[i].lenflds;
					break;
				default:
					/* FIXME: error */
					break;
				}
			} else { /* Fixed length */
				len = def[i].flds;

				/* FIXME: How can we check ISO_BINARY? */
				if (def[i].format != ISO_BINARY && 
				    strlen(pmsg->fld[i]) != len) {
					/* FIXME: error */
				}
			}

			memcpy(buf, pmsg->fld[i], len);

			buf += len;
                }
	}

	return (buf - start);
}

int unpack(isomsg *msg, const isodef *def, const char *buf) {
	isomsg_t *pmsg = (isomsg_t *) *msg;
	int flds;
	int i;
	int len;
        char tmp[20];

	/* Field 0 is mandatory and fixed length. */
	if (def[0].lenflds != 0) {
		/* FIXME: error */
	}
	pmsg->fld[0] = (char *) malloc((def[0].flds + 1) * sizeof(char));
	memcpy(pmsg->fld[0], buf, def[0].flds);
	pmsg->fld[0][def[0].flds] = 0;
	buf += def[0].flds;

	/*
	 * First bit in the bitmap (field 1) defines if the message is 
	 * extended or not.
	 * 0: not exteded - i.e. only (some of) the fields 0 to 64 are 
	 *    used
	 * 1: extended - i.e. (some of) the fields 0 to 128 are used
	 * I.e. if one or more of the fields 65 to 128 is used, the 
	 * bit is 1.
	 */
	if(buf[0] & 0x80) {
		flds = 128;
	} else {
		flds = 64;
	}
	pmsg->fld[1] = (char *) calloc(flds/8 + 1, sizeof(char));
	memcpy(pmsg->fld[1], buf, flds/8);
	buf += flds/8;

	for (i = 2; i <= flds; i++) {
		if ((pmsg->fld[1][(i-1)/8] << ((i-1)%8)) & 0x80) { /* i'th bit != 0 */
			if (def[i].lenflds) { /* Variable length */
				sprintf(tmp, "%%0%dd", def[i].lenflds);
				sscanf(buf, tmp, &len);
				/*
				 * The length of a field can't be 
				 * larger than defined by def[i].flds.
				 */
				if (len > def[i].flds) {
					/* FIXME: warning/error */
				}
			} else { /* Fixed length */
				len = def[i].flds;
			}

			switch (def[i].format) {
			case ISO_NUMERIC: /* Fallthrough */
			case ISO_ALPHANUMERIC:
				/* Don't copy lenght bytes */
				buf += def[i].lenflds;
				break;
			case ISO_BINARY:
				/* Copy length bytes */
				len += def[i].lenflds;
				break;
			default:
				/* FIXME: error */
				break;
			}

			pmsg->fld[i] = (char *) malloc((len + 1) * sizeof(char));
			memcpy(pmsg->fld[i], buf, len);
			pmsg->fld[i][len] = 0;

			buf += len;
		}
	}

	return 0;
}

int dump(FILE *fp, const isomsg *msg) {
	int i;
	isomsg_t *pmsg = (isomsg_t *) *msg;
	for (i = 0; i < 129; i++) {
		if (pmsg->fld[i] != NULL) {
			fprintf(fp, "field #%d = %s\n", i, pmsg->fld[i]);
		}
	}
}

void free_msg(isomsg *msg) {
	if (msg != NULL) {
		isomsg_t *pmsg = (isomsg_t *) *msg;	
		int i;
		for (i = 0; i < 129; i++) {
			if (pmsg->fld[i] != NULL) {
				free(pmsg->fld[i]);
				pmsg->fld[i] = NULL;
			}
		}
		
		free(pmsg);
	}
}