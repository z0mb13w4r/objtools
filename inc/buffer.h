#ifndef __BUFFER_H_
#define __BUFFER_H_

#include "common.h"

#define MODE_BUFFER             (MODE_PUT0('B') | MODE_PUT1('U') | MODE_PUT2('F'))

typedef struct buffer_s {
  smode_t  mode;
  char     note[256];

  size_t   size;
  void    *data;

} buffer_t, *pbuffer_t;

handle_t setmode(handle_t p, const int mode);
handle_t create(const int mode);
handle_t destroy(handle_t p);
handle_t release(handle_t p);

int ismode(handle_t p, const int mode);

unknown_t mallocx(const size_t size);

handle_t bmalloc();
handle_t bfree(handle_t p);
handle_t bopen(const char* name);

int issafe(pbuffer_t p);

void* getp(const pbuffer_t p, const int offset, const size_t size);
int   get(const pbuffer_t p, const int offset);

int isBigEndian(const pbuffer_t p);
int isLittleEndian(const pbuffer_t p);

#endif

