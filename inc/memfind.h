#ifndef __MEMFIND_H_
#define __MEMFIND_H_

#include "buffer.h"

#define MODE_FIND               (MODE_PUT0('F') | MODE_PUT1('N') | MODE_PUT2('D'))

typedef struct find_s {
  smode_t  mode;
  size_t   cpos, epos, chunksize;
  void    *item;

} find_t, *pfind_t;

int isfind(handle_t p);

void*    fget(handle_t p);
handle_t fnext(handle_t p);

handle_t fmalloc(void *data, const size_t size, const size_t chunksize);
handle_t ffree(handle_t p);

#endif

