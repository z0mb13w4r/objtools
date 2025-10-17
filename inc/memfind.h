#ifndef __MEMFIND_H_
#define __MEMFIND_H_

#include "buffer.h"

#define MODE_FIND               (MODE_PUT0('F') | MODE_PUT1('N') | MODE_PUT2('D'))

typedef struct find_s {
  smode_t   mode;
  size_t    cpos, epos, chunksize;
  unknown_t item;

} find_t, *pfind_t;

int isfind(handle_t p);

size_t fgetsize(handle_t p);

unknown_t fget(handle_t p);
unknown_t fmove(handle_t p, const size_t cpos);
unknown_t fgetp(handle_t p, const size_t chunksize);
unknown_t fstep(handle_t p, const size_t chunksize);
unknown_t fupdate(handle_t p, const size_t cpos, const size_t chunksize);

handle_t fnext(handle_t p);
handle_t fmalloc(unknown_t data, const size_t size, const size_t chunksize);
handle_t ffree(handle_t p);

int64_t  fgets8(handle_t p);
int64_t  fgets16(handle_t p);
int64_t  fgets32(handle_t p);
int64_t  fgets64(handle_t p);

uint64_t fgetu8(handle_t p);
uint64_t fgetu16(handle_t p);
uint64_t fgetu32(handle_t p);
uint64_t fgetu64(handle_t p);

uint64_t fgetuleb128(handle_t p);

char* fgetstring(handle_t p);

#endif

