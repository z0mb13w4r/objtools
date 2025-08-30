#ifndef __MEMUSE_H_
#define __MEMUSE_H_

#include "defines.h"

unknown_t xmalloc(const size_t size);
unknown_t xfree(unknown_t p);

unknown_t zfree(punknown_t p);

size_t xstrlen(const char *src);

char *xstrcpy(char *dst, const char *src);
char *xstrncpy(char *dst, const char *src, size_t count);

char* xstrdup(const char *src);
char* xstrndup(const char *src, size_t size);

unknown_t xmemcpy(unknown_t dst, const unknown_t src, size_t count);
unknown_t xmemset(unknown_t ptr, int value, size_t count);

const char* strshorten(const char* p);

#endif

