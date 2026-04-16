#include <zlib.h>

#include "zlib-memfind.h"

#define ZLIB_CHUNKSIZE                     (16384)
#define ZLIB_MAXCHUNKSIZE                  (65536)

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
  MALLOCA(unsigned char, out, ZLIB_CHUNKSIZE);

  if (isfind(p)) {
    handle_t q = fxalloc(ZLIB_MAXCHUNKSIZE, ZLIB_MAXCHUNKSIZE | MEMFIND_FILL);
    if (isfind(q)) {
      z_stream strm;

      /* allocate deflate state */
      strm.zalloc = Z_NULL;
      strm.zfree = Z_NULL;
      strm.opaque = Z_NULL;
      int ret = deflateInit(&strm, get_ZLIBLEVELS(mode));
      if (Z_OK == ret) {
        /* compress until end of file */
        while (!fiseof(p)) {
          strm.avail_in = MIN(ZLIB_CHUNKSIZE, fgetsize(p));
          strm.next_in = fgetp(p, strm.avail_in);

          /* run deflate() on input until output buffer not full, finish
             compression if all of source has been read in */
          do {
            strm.avail_out = ZLIB_CHUNKSIZE;
            strm.next_out = out;
            ret = deflate(&strm, fiseof(p) ? Z_FINISH : Z_NO_FLUSH);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

            fappendp(q, out, ZLIB_CHUNKSIZE - strm.avail_out);
          } while (strm.avail_out == 0);
          assert(strm.avail_in == 0);     /* all input will be used */

          /* done when last data in file processed */
        }
        assert(ret == Z_STREAM_END);        /* stream will be complete */

        /* clean up and return */
        deflateEnd(&strm);
      }

      return q;
    }
  }

  return NULL;
}

handle_t zlib_decompress(handle_t p) {
  if (isfind(p)) {
    handle_t q = fxalloc(ZLIB_MAXCHUNKSIZE, ZLIB_MAXCHUNKSIZE | MEMFIND_FILL);
    if (isfind(q)) {
      z_stream strm;

      return q;
    }
  }

  return NULL;
}

