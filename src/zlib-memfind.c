#include <zlib.h>

#include "zlib-memfind.h"

#define CHUNK 16384

handle_t zlib_compress(handle_t p, const imode_t mode) {
  int ret, flush;
  unsigned have;
  unsigned char in[CHUNK];
  unsigned char out[CHUNK];

  z_stream strm;

  /* allocate deflate state */
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
//  ret = deflateInit(&strm, level);
  if (Z_OK == ret) {
    /* compress until end of file */
    do {
//      strm.avail_in = fread(in, 1, CHUNK, source);
//      if (ferror(source)) {
//        (void)deflateEnd(&strm);
//        return Z_ERRNO;
//      }
//      flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
      strm.next_in = in;

      /* run deflate() on input until output buffer not full, finish
         compression if all of source has been read in */
      do {
        strm.avail_out = CHUNK;
        strm.next_out = out;
        ret = deflate(&strm, flush);    /* no bad return value */
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
        have = CHUNK - strm.avail_out;
//        if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//          (void)deflateEnd(&strm);
//          return Z_ERRNO;
//        }
      } while (strm.avail_out == 0);
      assert(strm.avail_in == 0);     /* all input will be used */

      /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    deflateEnd(&strm);
  }

  return NULL;
}

handle_t zlib_decompress(handle_t p, handle_t q) {
  return NULL;
}

