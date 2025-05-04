#include "buffer.h"
#include "encode.h"
#include "bstring.h"

extern uchar_t base64_map[];

static int hexN(int x) {
  x &= 0x0f;
  if (0 <= x && x <= 9)  return x + '0';
  return x - 10 + 'a';
}

handle_t hex8_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 6 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pbstring_t dst = bstrmallocsize(maxsize);
    if (dst) {
      int c = 0;
      puchar_t pdst = CAST(puchar_t, dst->data);
      pdst[c++] = '0';
      pdst[c++] = 'x';
      pdst[c++] = hexN((psrc[0] >> 0) & 0x0f);
      pdst[c++] = hexN((psrc[0] >> 4) & 0x0f);

      for (size_t i = 1; i < srcsize; ++i) {
        pdst[c++] = ',';
        pdst[c++] = ' ';
        pdst[c++] = '0';
        pdst[c++] = 'x';
        pdst[c++] = hexN(psrc[i] & 0x0f);
        pdst[c++] = hexN((psrc[i] >> 4) & 0x0f);
      }

      pdst[c] = '\0';   /* string padding character */
      return dst;
    }
  }

  return NULL;
}

handle_t hex16_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 4 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pbstring_t dst = bstrmallocsize(maxsize);
    if (dst) {
      int c = 0;
      puchar_t pdst = CAST(puchar_t, dst->data);
      pdst[c++] = '0';
      pdst[c++] = 'x';
      pdst[c++] = hexN((psrc[0] >> 0) & 0x0f);
      pdst[c++] = hexN((psrc[0] >> 4) & 0x0f);

      for (size_t i = 1; i < srcsize; ++i) {
        if (0 == (i % 2)) {
          pdst[c++] = ',';
          pdst[c++] = ' ';
          pdst[c++] = '0';
          pdst[c++] = 'x';
        }
        pdst[c++] = hexN(psrc[i] & 0x0f);
        pdst[c++] = hexN((psrc[i] >> 4) & 0x0f);
      }

      pdst[c] = '\0';   /* string padding character */
      return dst;
    }
  }

  return NULL;
}

handle_t hex32_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 3 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pbstring_t dst = bstrmallocsize(maxsize);
    if (dst) {
      int c = 0;
      puchar_t pdst = CAST(puchar_t, dst->data);
      pdst[c++] = '0';
      pdst[c++] = 'x';
      pdst[c++] = hexN((psrc[0] >> 0) & 0x0f);
      pdst[c++] = hexN((psrc[0] >> 4) & 0x0f);

      for (size_t i = 1; i < srcsize; ++i) {
        if (0 == (i % 4)) {
          pdst[c++] = ',';
          pdst[c++] = ' ';
          pdst[c++] = '0';
          pdst[c++] = 'x';
        }
        pdst[c++] = hexN(psrc[i] & 0x0f);
        pdst[c++] = hexN((psrc[i] >> 4) & 0x0f);
      }

      pdst[c] = '\0';   /* string padding character */
      return dst;
    }
  }

  return NULL;
}

handle_t base64_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 4 / 3 + 4;
    puchar_t psrc = CAST(puchar_t, src);

    pbstring_t dst = bstrmallocsize(maxsize);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->data);

      uchar_t tmp[3];
      int c = 0, j = 0;
      for (int i = 0; i < srcsize; ++i) {
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
      return dst;
    }
  }

  return NULL;
}

