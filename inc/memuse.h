#ifndef __MEMUSE_H_
#define __MEMUSE_H_

#include "defines.h"

#define MODE_HEAP             (MODE_PUT0('H') | MODE_PUT1('E') | MODE_PUT2('A') | MODE_PUT3('P'))
#define MODE_STACK            (MODE_PUT0('S') | MODE_PUT1('T') | MODE_PUT2('C') | MODE_PUT3('K'))

#define MEMSTACK(x,y)          MALLOCA(uchar_t,p##y,sizeof(memuse_t) + sizeof(x)); setmode(p##y, MODE_STACK); x* y = CAST(x*, p##y + sizeof(memuse_t))

typedef struct memuse_s {
  smode_t  mode;
  size_t   size;
  size_t   sizemax;

} memuse_t, *pmemuse_t;

bool_t ismode(handle_t p, const nmode_t mode);
bool_t ismode0(handle_t p, const nmode_t mode);
bool_t ismode1(handle_t p, const nmode_t mode);
bool_t ismode2(handle_t p, const nmode_t mode);
bool_t ismode3(handle_t p, const nmode_t mode);
bool_t ismodeNNN(handle_t p, const nmode_t mode);
bool_t ismodeNXXN(handle_t p, const nmode_t mode);

handle_t setmode(handle_t p, const nmode_t mode);

unknown_t xmalloc(const size_t size, const nmode_t mode);
unknown_t cmalloc(const unknown_t p, const size_t size, const nmode_t mode);
unknown_t xfree(unknown_t p);

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

