#include "buffer.h"
#include "encode.h"
#include "memfind.h"

extern uchar_t base32_map[];
extern int32_t base58_idx[128];
extern uchar_t base58_map[];
extern uchar_t base64_map[];

static uchar_t base32_ext[] = { 0, 2, 4, 5, 7 };

static int binN(int x) {
  return x ? '1' : '0';
}

static int decN(int x) {
  return (0 <= x && x <= 9) ? x + '0' : '?';
}

static int hexN(int x) {
  x &= 0x0f;
  if (0 <= x && x <= 9)  return x + '0';
  return x - 10 + 'a';
}

handle_t bin8_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t dstsize = srcsize * 10 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(dstsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);

      for (size_t i = 0; i < srcsize; ++i) {
        if (0 != i) {
          pdst[dst->cpos++] = ',';
          pdst[dst->cpos++] = ' ';
        }

        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(7));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(6));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(5));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(4));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(3));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(2));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(1));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(0));
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

handle_t bin16_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t dstsize = srcsize * 10 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(dstsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(7));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(6));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(5));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(4));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(3));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(2));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(1));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(0));

      for (size_t i = 1; i < srcsize; ++i) {
        if (0 == (i % 2)) {
          pdst[dst->cpos++] = ',';
          pdst[dst->cpos++] = ' ';
        }
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(7));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(6));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(5));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(4));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(3));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(2));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(1));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(0));
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

handle_t bin32_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t dstsize = srcsize * 10 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(dstsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(7));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(6));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(5));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(4));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(3));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(2));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(1));
      pdst[dst->cpos++] = binN(psrc[0] & U32MASK(0));

      for (size_t i = 1; i < srcsize; ++i) {
        if (0 == (i % 4)) {
          pdst[dst->cpos++] = ',';
          pdst[dst->cpos++] = ' ';
        }
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(7));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(6));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(5));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(4));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(3));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(2));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(1));
        pdst[dst->cpos++] = binN(psrc[i] & U32MASK(0));
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

handle_t dec8_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t dstsize = srcsize * 5 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(dstsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);

      for (size_t i = 0; i < srcsize; ++i) {
        uchar_t v2 = psrc[i];
        uchar_t v0 = v2 % 10; v2 /= 10;
        uchar_t v1 = v2 % 10; v2 /= 10;

        if (0 != i) {
          pdst[dst->cpos++] = ',';
          pdst[dst->cpos++] = ' ';
        }

        size_t cpos = dst->cpos;
        if (v2) {
          pdst[dst->cpos++] = decN(v2);
        }
        if (v1 || cpos != dst->cpos) {
          pdst[dst->cpos++] = decN(v1);
        }
        if (v0 || cpos != dst->cpos) {
          pdst[dst->cpos++] = decN(v0);
        }
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

handle_t dec16_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t minsize = srcsize / 2;
    size_t maxsize = (srcsize + 1) / 2;
    size_t dstsize = srcsize * 8 + 1;
    pushort_t psrc = CAST(pushort_t, src);

    pfind_t dst = fxalloc(dstsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);

      for (size_t i = 0; i < minsize; ++i) {
        ushort_t v5 = psrc[i];
        ushort_t v0 = v5 % 10; v5 /= 10;
        ushort_t v1 = v5 % 10; v5 /= 10;
        ushort_t v2 = v5 % 10; v5 /= 10;
        ushort_t v3 = v5 % 10; v5 /= 10;
        ushort_t v4 = v5 % 10; v5 /= 10;

        if (0 != i) {
          pdst[dst->cpos++] = ',';
          pdst[dst->cpos++] = ' ';
        }

        size_t cpos = dst->cpos;
        if (v5) {
          pdst[dst->cpos++] = decN(v5);
        }
        if (v4) {
          pdst[dst->cpos++] = decN(v4);
        }
        if (v3) {
          pdst[dst->cpos++] = decN(v3);
        }
        if (v2) {
          pdst[dst->cpos++] = decN(v2);
        }
        if (v1 || cpos != dst->cpos) {
          pdst[dst->cpos++] = decN(v1);
        }
        if (v0 || cpos != dst->cpos) {
          pdst[dst->cpos++] = decN(v0);
        }
      }

      if (minsize != maxsize) {
        pdst[dst->cpos++] = ',';
        pdst[dst->cpos++] = ' ';

        ushort_t v2 = CAST(puchar_t, src)[srcsize - 1];
        ushort_t v0 = v2 % 10; v2 /= 10;
        ushort_t v1 = v2 % 10; v2 /= 10;

        size_t cpos = dst->cpos;
        if (v2) {
          pdst[dst->cpos++] = decN(v2);
        }
        if (v1 || cpos != dst->cpos) {
          pdst[dst->cpos++] = decN(v1);
        }
        if (v0 || cpos != dst->cpos) {
          pdst[dst->cpos++] = decN(v0);
        }
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

handle_t dec32_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {

  }

  return NULL;
}

handle_t hex8_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t dstsize = srcsize * 6 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(dstsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);
      pdst[dst->cpos++] = '0';
      pdst[dst->cpos++] = 'x';
      pdst[dst->cpos++] = hexN((psrc[0] >> 0) & 0x0f);
      pdst[dst->cpos++] = hexN((psrc[0] >> 4) & 0x0f);

      for (size_t i = 1; i < srcsize; ++i) {
        pdst[dst->cpos++] = ',';
        pdst[dst->cpos++] = ' ';
        pdst[dst->cpos++] = '0';
        pdst[dst->cpos++] = 'x';
        pdst[dst->cpos++] = hexN((psrc[i] >> 0) & 0x0f);
        pdst[dst->cpos++] = hexN((psrc[i] >> 4) & 0x0f);
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

handle_t hex16_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t dstsize = srcsize * 4 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(dstsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);
      pdst[dst->cpos++] = '0';
      pdst[dst->cpos++] = 'x';
      pdst[dst->cpos++] = hexN((psrc[0] >> 0) & 0x0f);
      pdst[dst->cpos++] = hexN((psrc[0] >> 4) & 0x0f);

      for (size_t i = 1; i < srcsize; ++i) {
        if (0 == (i % 2)) {
          pdst[dst->cpos++] = ',';
          pdst[dst->cpos++] = ' ';
          pdst[dst->cpos++] = '0';
          pdst[dst->cpos++] = 'x';
        }
        pdst[dst->cpos++] = hexN((psrc[i] >> 0) & 0x0f);
        pdst[dst->cpos++] = hexN((psrc[i] >> 4) & 0x0f);
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

handle_t hex32_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t dstsize = srcsize * 3 + 1;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(dstsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);
      pdst[dst->cpos++] = '0';
      pdst[dst->cpos++] = 'x';
      pdst[dst->cpos++] = hexN((psrc[0] >> 0) & 0x0f);
      pdst[dst->cpos++] = hexN((psrc[0] >> 4) & 0x0f);

      for (size_t i = 1; i < srcsize; ++i) {
        if (0 == (i % 4)) {
          pdst[dst->cpos++] = ',';
          pdst[dst->cpos++] = ' ';
          pdst[dst->cpos++] = '0';
          pdst[dst->cpos++] = 'x';
        }
        pdst[dst->cpos++] = hexN((psrc[i] >> 0) & 0x0f);
        pdst[dst->cpos++] = hexN((psrc[i] >> 4) & 0x0f);
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

static void base32_encode0(const puchar_t src, puchar_t dst, size_t size) {
  if (5 <= size) {
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
    size_t maxsize = srcsize * 4 / 3 + 4;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(maxsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);
      puchar_t ptmp = xmalloc(srcsize * 137 / 100);
      if (ptmp) {
        size_t tmpsize = 1;
        for (size_t i = 0; i < srcsize; ++i) {
          int32_t carry = CAST(int32_t, psrc[i]);
          for (size_t j = 0; j < tmpsize; j++) {
            carry += CAST(int32_t, ptmp[j]) << 8;
            ptmp[j] = CAST(uchar_t, carry % 58);
            carry /= 58;
          }

          while (carry > 0) {
            ptmp[tmpsize++] = CAST(uchar_t, carry % 58);
            carry /= 58;
          }
        }

        // leading zero bytes
        for (dst->cpos = 0; dst->cpos < srcsize && psrc[dst->cpos] == 0; ) {
          pdst[dst->cpos++] = '1';
        }

        // reverse
        for (size_t i = 0; i < tmpsize; ++i) {
          pdst[dst->cpos++] = base58_map[ptmp[tmpsize - 1 - i]];
        }

        xfree(ptmp);
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

handle_t base64_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 4 / 3 + 4;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(maxsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);

      uchar_t tmp[3];
      int j = 0;
      for (int i = 0; i < srcsize; ++i) {
        tmp[j++] = psrc[i];
        if (j == 3) {
          pdst[dst->cpos++] = base64_map[tmp[0] >> 2];
          pdst[dst->cpos++] = base64_map[((tmp[0] & 0x03) << 4) + (tmp[1] >> 4)];
          pdst[dst->cpos++] = base64_map[((tmp[1] & 0x0f) << 2) + (tmp[2] >> 6)];
          pdst[dst->cpos++] = base64_map[tmp[2] & 0x3f];
          j = 0;
        }
      }

      if (j > 0) {
        pdst[dst->cpos++] = base64_map[tmp[0] >> 2];
        if (j == 1) {
          pdst[dst->cpos++] = base64_map[(tmp[0] & 0x03) << 4];
          pdst[dst->cpos++] = '=';
        } else {
          pdst[dst->cpos++] = base64_map[((tmp[0] & 0x03) << 4) + (tmp[1] >> 4)];
          pdst[dst->cpos++] = base64_map[(tmp[1] & 0x0f) << 2];
        }
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

