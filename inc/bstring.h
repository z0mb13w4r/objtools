#ifndef __BSTRING_H
#define __BSTRING_H

#include "defines.h"

#include "defines.h"

#define MODE_BSTRING             (MODE_PUT0('S') | MODE_PUT1('T') | MODE_PUT2('R'))

#define BSTRSPOS                 (0)
#define BSTREPOS                 (~0)

typedef struct bstring_s {
  smode_t  mode;
  size_t   size, cpos;
  void    *data;

} bstring_t, *pbstring_t;

bool_t isbstring(handle_t p);

handle_t bstrmalloc();
handle_t bstrmallocsize(const size_t size);
handle_t bstrfree(handle_t p);

size_t bstrlen(handle_t p);
size_t bstrsize(handle_t p);

handle_t bstrclr(handle_t p);
handle_t bstrlwr(handle_t p);
handle_t bstrupr(handle_t p);
handle_t bstrcat(handle_t dst, handle_t src);
handle_t bstrcpy(handle_t dst, handle_t src);
handle_t bstrncat(handle_t dst, handle_t src, size_t size);
handle_t bstrncpy(handle_t dst, handle_t src, size_t size);

int bstrcmp(handle_t s1, handle_t s2);
int bstrncmp(handle_t s1, handle_t s2, size_t size);
int bstrcasecmp(handle_t s1, handle_t s2);
int bstrncasecmp(handle_t s1, handle_t s2, size_t size);

#endif

