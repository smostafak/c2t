#ifndef CONVERT_H_
#define CONVERT_H_
#include <stdio.h>
#include "expat.h"
#include "iso8583.h"

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#define BUFFSIZE        8192
#define TMPSIZE		512

static void XMLCALL
handle_start(void *data, const char *el, const char **attr);

static void XMLCALL
handle_end(void *data, const char *el);
#endif /*CONVERT_H_*/
