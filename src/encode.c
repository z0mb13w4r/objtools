#include "buffer.h"
#include "encode.h"
#include "memfind.h"

extern uchar_t base32_map[];
extern uchar_t base64_map[];

static uchar_t base32_ext[] = { 0, 2, 4, 5, 7 };

static int hexN(int x) {
  x &= 0x0f;
  if (0 <= x && x <= 9)  return x + '0';
  return x - 10 + 'a';
}

handle_t hex8_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 6 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(maxsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      int c = 0;
      puchar_t pdst = CAST(puchar_t, dst->item);
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

    pfind_t dst = fxalloc(maxsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      int c = 0;
      puchar_t pdst = CAST(puchar_t, dst->item);
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

    pfind_t dst = fxalloc(maxsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      int c = 0;
      puchar_t pdst = CAST(puchar_t, dst->item);
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

static void base32_encode0(const puchar_t src, puchar_t dst, size_t size) {
  uint32_t hi = (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3];
  uint32_t lo = (hi << 8) | src[4];

  dst[0] = base64_map[hi >> 27 & 0x1f];
  dst[1] = base64_map[hi >> 22 & 0x1f];
  dst[2] = base64_map[hi >> 17 & 0x1f];
  dst[3] = base64_map[hi >> 12 & 0x1f];
  dst[4] = base64_map[hi >> 7 & 0x1f];
  dst[5] = base64_map[hi >> 2 & 0x1f];
  dst[6] = base64_map[lo >> 5 & 0x1f];
  dst[7] = base64_map[lo & 0x1f];
}

handle_t base32_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 3 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(maxsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);

      size_t si = 0;
      size_t sz = (srcsize / 5) * 5;
      size_t sx = srcsize - sz;

      while (si < sz) {
        base32_encode0(psrc + si, pdst + dst->cpos, sz - si);
        si += 5;
        dst->cpos += 8;
      }

      uint32_t vx = 0;

      switch (sx) {
      case 4:
        vx |= psrc[si + 3];
        pdst[dst->cpos + 6] = base64_map[vx << 3 & 0x1f];
        pdst[dst->cpos + 5] = base64_map[vx >> 2 & 0x1f];

      case 3:
        vx |= psrc[si + 2] << 8;
        pdst[dst->cpos + 4] = base64_map[vx >> 7 & 0x1f];

      case 2:
        vx |= psrc[si + 1] << 16;
        pdst[dst->cpos + 3] = base64_map[vx >> 12 & 0x1f];
        pdst[dst->cpos + 2] = base64_map[vx >> 17 & 0x1f];

      case 1:
        vx |= psrc[si + 0] << 24;
        pdst[dst->cpos + 1] = base64_map[vx >> 22 & 0x1f];
        pdst[dst->cpos + 0] = base64_map[vx >> 27 & 0x1f];
      }

      if (sx < NELEMENTS(base32_ext)) {
        dst->cpos += base32_ext[sx];
      }

      size_t npad = (sx * 8 / 5) + 1;
      for (size_t i = npad; i < 8; ++i) {
        pdst[dst->cpos++] = '=';
      }

      pdst[dst->cpos] = '\0';   /* string padding character */
      dst->size = dst->cpos + 1;
      dst->epos = dst->cpos;
      dst->cpos = 0;
      return dst;
    }
  }

  return NULL;
}

handle_t base58_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
  }

  return NULL;
}

handle_t base64_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 4 / 3 + 4;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(maxsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);

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

