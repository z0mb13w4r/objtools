#ifndef __BUFFER_H_
#define __BUFFER_H_

#include "memuse.h"

#define MODE_BUFFER             (MODE_PUT0('B') | MODE_PUT1('U') | MODE_PUT2('F'))

typedef struct buffer_s {
  smode_t  mode;
  char     note[256];

  size_t   size;
  void    *data;

} buffer_t, *pbuffer_t;

bool_t issafe(pbuffer_t p);
bool_t isbuffer(handle_t p);

unknown_t ndump(unknown_t p);
unknown_t nfree(unknown_t p);

size_t xget(unknown_t p, size_t size, size_t count, unknown_t f);
size_t xput(unknown_t p, size_t size, size_t count, unknown_t f);
size_t xset(unknown_t p, size_t size, const char* name);

size_t bgetsize(handle_t p);

unknown_t bget(handle_t p);

handle_t bmalloc();
handle_t bmallocsize(const size_t size);
handle_t bfree(handle_t p);
handle_t bopen(const char* name);
handle_t bclone(handle_t p, const int offset, const size_t size);
handle_t bappend(handle_t p, unknown_t px, const size_t size);
handle_t bresize(handle_t p, const size_t size);

unknown_t getp(const pbuffer_t p, const int offset, const size_t size);
int       getb(const pbuffer_t p, const int offset);

#endif

