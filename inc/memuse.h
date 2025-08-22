#ifndef __MEMUSE_H_
#define __MEMUSE_H_

#include "defines.h"

unknown_t xmalloc(const size_t size);
unknown_t xfree(unknown_t p);

unknown_t zfree(punknown_t p);

char* xstrdup(const char *s);
char* xstrndup(const char *s, size_t size);

#endif

