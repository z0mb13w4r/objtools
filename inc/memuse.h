#ifndef __MEMUSE_H_
#define __MEMUSE_H_

#include "defines.h"

unknown_t xmalloc(const size_t size);
unknown_t cmalloc(const unknown_t p, const size_t size);
unknown_t xfree(unknown_t p);

unknown_t zfree(punknown_t p);

int xstrcmp(const char *x, const char *y);
int xstrncmp(const char *x, const char *y, size_t count);

int xstrichr(const char *str, int ch);

size_t xstrlen(const char *s);
size_t xstrnlen(const char *s, size_t count);

char *xstrcat(char *dst, const char *src);
char *xstrncat(char *dst, const char *src, size_t count);

char *xstrchr(const char* str, int ch);

char *xstrcpy(char *dst, const char *src);
char *xstrncpy(char *dst, const char *src, size_t count);

char* xstrdup(const char *str);
char* xstrndup(const char *str, size_t size);

char* xstrgrab(const char* name, int spos, int epos);

uchar_t  xstrcrc8(const char* src);
uchar_t  xstrncrc8(const char* src, size_t count);
ushort_t xstrcrc16(const char* src);
ushort_t xstrncrc16(const char* src, size_t count);
ulong_t  xstrcrc32(const char* src);
ulong_t  xstrncrc32(const char* src, size_t count);

unknown_t xmemcpy(unknown_t dst, const unknown_t src, size_t count);
unknown_t xmemset(unknown_t ptr, int value, size_t count);
unknown_t xmemclr(unknown_t ptr, size_t count);

const char* strshorten(const char* p);

#endif

