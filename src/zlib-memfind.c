#include <zlib.h>

#include "zlib-memfind.h"

#define CHUNK                      (16384)

typedef struct swapmode_s {
  int     mode0;
  imode_t mode1;
} swapmode_t, *pswapmode_t;

static swapmode_t zZLIBLEVELS[] = {
  {Z_NO_COMPRESSION,           ZLIB_NO_COMPRESSION},
  {Z_BEST_SPEED,               ZLIB_BEST_SPEED},
  {Z_BEST_COMPRESSION,         ZLIB_BEST_COMPRESSION},
  {Z_DEFAULT_COMPRESSION,      ZLIB_DEFAULT_COMPRESSION},

  {0, 0}
};

static int get_swapmode(const pswapmode_t p, const imode_t mode, const int def) {
  if (p) {
    for (pswapmode_t pp = p; 0 != pp->mode1; ++pp) {
      if (mode == pp->mode1) return pp->mode0;
    }
  }

  return def;
}

static int get_ZLIBLEVELS(const imode_t mode) {
  return get_swapmode(zZLIBLEVELS, mode, Z_DEFAULT_COMPRESSION);
}

handle_t zlib_compress(handle_t p, const imode_t mode) {
  int ret, flush;
  unsigned have;
  unsigned char in[CHUNK];
  unsigned char out[CHUNK];

  if (isfind(p)) {
    z_stream strm;

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, get_ZLIBLEVELS(mode));
    if (Z_OK == ret) {
      /* compress until end of file */
      do {
//        strm.avail_in = fread(in, 1, CHUNK, source);
//        if (ferror(source)) {
//          (void)deflateEnd(&strm);
//          return Z_ERRNO;
//        }
//        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
          strm.avail_out = CHUNK;
          strm.next_out = out;
          ret = deflate(&strm, flush);    /* no bad return value */
          assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
          have = CHUNK - strm.avail_out;
//          if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//            (void)deflateEnd(&strm);
//            return Z_ERRNO;
//          }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
      } while (flush != Z_FINISH);
      assert(ret == Z_STREAM_END);        /* stream will be complete */

      /* clean up and return */
      deflateEnd(&strm);
    }
  }

  return NULL;
}

handle_t zlib_decompress(handle_t p, handle_t q) {
  return NULL;
}

