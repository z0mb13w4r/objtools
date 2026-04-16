#ifndef __ZLIB_MEMFIND_H_
#define __ZLIB_MEMFIND_H_

#define ZLIB_NO_COMPRESSION        (1)
#define ZLIB_BEST_SPEED            (2)
#define ZLIB_BEST_COMPRESSION      (3)
#define ZLIB_DEFAULT_COMPRESSION   (4)

#include "memfind.h"

handle_t zlib_compress(handle_t p, const imode_t mode);
handle_t zlib_decompress(handle_t p, handle_t q);

#endif

