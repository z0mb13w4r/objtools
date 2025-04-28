#ifndef __BSTRING_H
#define __BSTRING_H

#include "defines.h"

#include "defines.h"

#define MODE_STRING              (MODE_PUT0('S') | MODE_PUT1('T') | MODE_PUT2('R'))

#define BSTRSPOS                 (0)
#define BSTREPOS                 (~0)

typedef struct bstring_s {
  smode_t  mode;
  size_t   size, cpos;
  void    *data;

} bstring_t, *pbstring_t;

handle_t bstrmalloc();
handle_t bstrmallocsize(const size_t size);
handle_t bstrfree(handle_t p);

#endif

