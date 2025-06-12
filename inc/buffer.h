#ifndef __BUFFER_H_
#define __BUFFER_H_

#include "defines.h"

#define MODE_BUFFER             (MODE_PUT0('B') | MODE_PUT1('U') | MODE_PUT2('F'))

typedef struct buffer_s {
  smode_t  mode;
  char     note[256];

  size_t   size;
  void    *data;

} buffer_t, *pbuffer_t;

bool_t ismode(handle_t p, const nmode_t mode);
bool_t ismode0(handle_t p, const nmode_t mode);
bool_t ismode1(handle_t p, const nmode_t mode);
bool_t ismode2(handle_t p, const nmode_t mode);
bool_t ismode3(handle_t p, const nmode_t mode);
bool_t ismodeNNN(handle_t p, const nmode_t mode);
bool_t ismodeNXXN(handle_t p, const nmode_t mode);

handle_t setmode(handle_t p, const nmode_t mode);

bool_t issafe(pbuffer_t p);
bool_t isbuffer(handle_t p);

unknown_t xmalloc(const size_t size);
unknown_t xdump(unknown_t p);
unknown_t xfree(unknown_t p);
unknown_t zfree(punknown_t p);

size_t xget(unknown_t p, size_t size, size_t count, unknown_t f);
size_t xput(unknown_t p, size_t size, size_t count, unknown_t f);

handle_t bmalloc();
handle_t bmallocsize(const size_t size);
handle_t bfree(handle_t p);
handle_t bopen(const char* name);
handle_t bclone(handle_t p, const int offset, const size_t size);
handle_t bappend(handle_t p, unknown_t px, const size_t size);
handle_t bresize(handle_t p, const size_t size);

unknown_t getp(const pbuffer_t p, const int offset, const size_t size);
int       getb(const pbuffer_t p, const int offset);

int isBigEndian(const pbuffer_t p);
int isLittleEndian(const pbuffer_t p);

#endif

