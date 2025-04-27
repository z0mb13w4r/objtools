#include "buffer.h"
#include "encode.h"

extern uchar_t base64_map[];

unknown_t base64_encode(unknown_t src, size_t srcsize, unknown_t dst, size_t dstsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 4 / 3 + 4;
    if (0 == dstsize || maxsize <= dstsize) {
      puchar_t psrc = CAST(puchar_t, src);
      puchar_t pdst = CAST(puchar_t, dst);

      if (NULL == pdst) pdst = xmalloc(maxsize);

      uchar_t tmp[3];
      int i = 0, c = 0, j = 0;
      for (i = 0; i < srcsize; ++i) {
        tmp[j++] = psrc[i];
        if (j == 3) {
          pdst[c++] = base64_map[tmp[0] >> 2];
          pdst[c++] = base64_map[((tmp[0] & 0x03) << 4) + (tmp[1] >> 4)];
          pdst[c++] = base64_map[((tmp[1] & 0x0f) << 2) + (tmp[2] >> 6)];
          pdst[c++] = base64_map[tmp[2] & 0x3f];
          j = 0;
        }
      }

      if (j > 0) {
        pdst[c++] = base64_map[tmp[0] >> 2];
        if (j == 1) {
          pdst[c++] = base64_map[(tmp[0] & 0x03) << 4];
          pdst[c++] = '=';
        } else {
          pdst[c++] = base64_map[((tmp[0] & 0x03) << 4) + (tmp[1] >> 4)];
          pdst[c++] = base64_map[(tmp[1] & 0x0f) << 2];
        }
        pdst[c++] = '=';
      }

      pdst[c] = '\0';   /* string padding character */
      return pdst;
    }
  }

  return NULL;
}

