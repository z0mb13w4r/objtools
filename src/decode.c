#include <ctype.h>
#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
//#include <openssl/err.h>

#include "buffer.h"
#include "decode.h"
#include "memfind.h"
#include "objutils.h"

uchar_t base32_map[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '2', '3', '4', '5', '6', '7'
};

static uchar_t base32_ext[] = { 0, 0, 1, 0, 2, 3, 0, 4, 5 };

int32_t base58_idx[128] = {
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  -1,  0,  1,  2,  3,  4,  5,  6,  7,  8, -1, -1, -1, -1, -1, -1,
  -1,  9, 10, 11, 12, 13, 14, 15, 16, -1, 17, 18, 19, 20, 21, -1,
  22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, -1, -1, -1, -1, -1,
  -1, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, -1, 44, 45, 46,
  47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, -1, -1, -1, -1, -1
};

uchar_t base58_map[] = {
  '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
  'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
  'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p',
  'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

uchar_t base64_map[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

static const bool_t   base85_decode_zero = TRUE;
static const bool_t   base85_check_chars = TRUE;
static const uchar_t  base85_def = 33u;
static const uchar_t  base85_max = 85u;
static const uint32_t base85_maxchunk = 256u;

static int zap(int c0, int c1, int c2, int inc) {
  if (c0 >= c1 && c0 <= c2) {
    c0 += inc;
    if (c0 > c2) c0 = c1 + (c0 - c2) - 1;
  }

  return c0;
}

int rot5(const unknown_t p, const size_t size) {
  if (p) {
    char* p0 = CAST(char*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = zap(p0[i], '0', '9', 5);
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int rot13(const unknown_t p, const size_t size) {
  if (p) {
    char *p0 = CAST(char*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = zap(p0[i], 'A', 'Z', 13);
      p0[i] = zap(p0[i], 'a', 'z', 13);
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int rot18(const unknown_t p, const size_t size) {
  if (p) {
    char *p0 = CAST(char*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = zap(p0[i], 'A', 'Z', 13);
      p0[i] = zap(p0[i], 'a', 'z', 13);
      p0[i] = zap(p0[i], '0', '9', 5);
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int rot47(const unknown_t p, const size_t size) {
  if (p) {
    char *p0 = CAST(char*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = zap(p0[i], 33, 126, 47);
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int not8(const unknown_t p, const size_t size) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = ~p0[i];
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int not16(const unknown_t p, const size_t size) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      p0[i] = ~p0[i];
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int not32(const unknown_t p, const size_t size) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      p0[i] = ~p0[i];
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int shl8(const unknown_t p, const uint8_t v, const size_t size, const int32_t step) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    uint8_t  v0 = v;
    for (size_t i = 0; i < size; ++i) {
      p0[i] <<= v0;
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int shl16(const unknown_t p, const uint16_t v, const size_t size, const int32_t step) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    uint16_t  v0 = v;
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      p0[i] <<= v0;
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int shl32(const unknown_t p, const uint32_t v, const size_t size, const int32_t step) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    uint32_t  v0 = v;
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      p0[i] <<= v0;
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int shr8(const unknown_t p, const uint8_t v, const size_t size, const int32_t step) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    uint8_t  v0 = v;
    for (size_t i = 0; i < size; ++i) {
      p0[i] >>= v0;
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int shr16(const unknown_t p, const uint16_t v, const size_t size, const int32_t step) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    uint16_t  v0 = v;
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      p0[i] >>= v0;
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int shr32(const unknown_t p, const uint32_t v, const size_t size, const int32_t step) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    uint32_t  v0 = v;
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      p0[i] >>= v0;
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int rol8(const unknown_t p, const uint8_t v, const size_t size, const int32_t step) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    uint8_t  v0 = v;
    for (size_t i = 0; i < size; ++i) {
      p0[i] = (p0[i] << v0) | (p0[i] >> (8 - v0));
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int rol16(const unknown_t p, const uint16_t v, const size_t size, const int32_t step) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    uint16_t  v0 = v;
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      p0[i] = (p0[i] << v0) | (p0[i] >> (16 - v0));
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int rol32(const unknown_t p, const uint32_t v, const size_t size, const int32_t step) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    uint32_t  v0 = v;
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      p0[i] = (p0[i] << v0) | (p0[i] >> (32 - v0));
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int ror8(const unknown_t p, const uint8_t v, const size_t size, const int32_t step) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    uint8_t  v0 = v;
    for (size_t i = 0; i < size; ++i) {
      p0[i] = (p0[i] >> v0) | (p0[i] << (8 - v0));
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int ror16(const unknown_t p, const uint16_t v, const size_t size, const int32_t step) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    uint16_t  v0 = v;
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      p0[i] = (p0[i] >> v0) | (p0[i] << (16 - v0));
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int ror32(const unknown_t p, const uint32_t v, const size_t size, const int32_t step) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    uint32_t  v0 = v;
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      p0[i] = (p0[i] >> v0) | (p0[i] << (32 - v0));
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int xor8(const unknown_t p, const uint8_t v, const size_t size, const int32_t step) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    uint8_t  v0 = v;
    for (size_t i = 0; i < size; ++i) {
      p0[i] ^= v0;
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int xor16(const unknown_t p, const uint16_t v, const size_t size, const int32_t step) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    uint16_t  v0 = v;
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      p0[i] ^= v0;
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int xor32(const unknown_t p, const uint32_t v, const size_t size, const int32_t step) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    uint32_t  v0 = v;
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      p0[i] ^= v0;
      v0 += step;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int add8(const unknown_t p, const uint8_t v, const size_t size) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    for (size_t i = 0; i < size; ++i) {
      const int32_t v0 = p0[i] + v;
      p0[i] = v0 > UCHAR_MAX ? v0 - UCHAR_MAX : v0;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int add16(const unknown_t p, const uint16_t v, const size_t size) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      const int32_t v0 = p0[i] + v;
      p0[i] = v0 > USHRT_MAX ? v0 - USHRT_MAX : v0;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int add32(const unknown_t p, const uint32_t v, const size_t size) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      const int64_t v0 = p0[i] + v;
      p0[i] = v0 > ULONG_MAX ? v0 - ULONG_MAX : v0;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int sub8(const unknown_t p, const uint8_t v, const size_t size) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    for (size_t i = 0; i < size; ++i) {
      const int32_t v0 = p0[i] - v;
      p0[i] = v0 < 0 ? UCHAR_MAX + v0 : v0;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int sub16(const unknown_t p, const uint16_t v, const size_t size) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      const int32_t v0 = p0[i] - v;
      p0[i] = v0 < 0 ? USHRT_MAX + v0 : v0;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int sub32(const unknown_t p, const uint32_t v, const size_t size) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      const int64_t v0 = p0[i] - v;
      p0[i] = v0 < 0 ? ULONG_MAX + v0 : v0;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int aes_encrypt(const int mode, puchar_t src, const size_t srcsize,
                puchar_t key, const size_t keysize, puchar_t ivec, puchar_t dst) {

//   ERR_load_crypto_strings();
//   OpenSSL_add_all_algorithms();

  int size = 0;
  int dstsize = 0;

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (NULL == ctx) return ECODE_CRYPTO;

  int x = TRUE;

  switch (mode) {
  case AES_MODE_ECB128:      x = EVP_EncryptInit_ex(ctx, EVP_aes_128_ecb(), NULL, key, NULL);  break;
  case AES_MODE_ECB192:      x = EVP_EncryptInit_ex(ctx, EVP_aes_192_ecb(), NULL, key, NULL);  break;
  case AES_MODE_ECB256:      x = EVP_EncryptInit_ex(ctx, EVP_aes_256_ecb(), NULL, key, NULL);  break;
  case AES_MODE_CBC128:      x = EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, ivec);  break;
  case AES_MODE_CBC192:      x = EVP_EncryptInit_ex(ctx, EVP_aes_192_cbc(), NULL, key, ivec);  break;
  case AES_MODE_CBC256:      x = EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, ivec);  break;
  case AES_MODE_CFB128:      x = EVP_EncryptInit_ex(ctx, EVP_aes_128_cfb(), NULL, key, ivec);  break;
  case AES_MODE_CFB192:      x = EVP_EncryptInit_ex(ctx, EVP_aes_192_cfb(), NULL, key, ivec);  break;
  case AES_MODE_CFB256:      x = EVP_EncryptInit_ex(ctx, EVP_aes_256_cfb(), NULL, key, ivec);  break;
  case AES_MODE_XTS128:      x = EVP_EncryptInit_ex(ctx, EVP_aes_128_xts(), NULL, key, ivec);  break;
  case AES_MODE_XTS256:      x = EVP_EncryptInit_ex(ctx, EVP_aes_256_xts(), NULL, key, ivec);  break;
  case AES_MODE_OFB128:      x = EVP_EncryptInit_ex(ctx, EVP_aes_128_ofb(), NULL, key, ivec);  break;
  case AES_MODE_OFB192:      x = EVP_EncryptInit_ex(ctx, EVP_aes_192_ofb(), NULL, key, ivec);  break;
  case AES_MODE_OFB256:      x = EVP_EncryptInit_ex(ctx, EVP_aes_256_ofb(), NULL, key, ivec);  break;
  case AES_MODE_CTR128:      x = EVP_EncryptInit_ex(ctx, EVP_aes_128_ctr(), NULL, key, ivec);  break;
  case AES_MODE_CTR192:      x = EVP_EncryptInit_ex(ctx, EVP_aes_192_ctr(), NULL, key, ivec);  break;
  case AES_MODE_CTR256:      x = EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, key, ivec);  break;
  default:
    break;
  }

  if (0 != x) {
    EVP_CIPHER_CTX_set_padding(ctx, 0);
  }

  if (0 != x) {
    x =  EVP_EncryptUpdate(ctx, dst, &size, src, srcsize);
    dstsize += size;
  }

  if (0 != x) {
    x = EVP_EncryptFinal_ex(ctx, dst + size, &size);
    dstsize += size;
  }

  EVP_CIPHER_CTX_free(ctx);

  return dstsize;
}

handle_t bin_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(srcsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      dst->cpos = -1;
      puchar_t pdst = CAST(puchar_t, dst->item);

      size_t isok = 0;
      for (size_t i = 0; i < srcsize; ++i) {
        uchar_t ch = psrc[i];
        bool_t isbin = isbin8(ch);
        if (isbin) {
          if (0 == (isok++ & 0x07)) ++dst->cpos;
          pdst[dst->cpos] = (pdst[dst->cpos] << 1) | bin8(ch);
        } else {
          isok = 0;
        }
      }

      pdst[++dst->cpos] = '\0';   /* string padding character */
      dst->size = dst->cpos + 1;
      dst->epos = dst->cpos;
      dst->cpos = 0;
      return dst;
    }
  }

  return NULL;
}

handle_t dec8_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(srcsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      dst->cpos = -1;
      puchar_t pdst = CAST(puchar_t, dst->item);

      bool_t isok = FALSE;
      for (size_t i = 0; i < srcsize; ++i) {
        uchar_t ch = psrc[i];
        bool_t isnum = isdec8(ch);
        if (isnum) {
          if (!isok) ++dst->cpos;
          pdst[dst->cpos] = (pdst[dst->cpos] * 10) + dec8(ch);
        }
        isok = isnum;
      }

      pdst[++dst->cpos] = '\0';   /* string padding character */
      dst->size = dst->cpos;
      dst->epos = dst->cpos - 1;
      dst->cpos = 0;
      return dst;
    }
  }

  return NULL;
}

handle_t dec16_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(srcsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      int c = -1;
      pushort_t pdst = CAST(pushort_t, dst->item);

      bool_t isok = FALSE;
      for (size_t i = 0; i < srcsize; ++i) {
        uchar_t ch = psrc[i];
        bool_t isnum = isdec8(ch);
        if (isnum) {
          if (!isok) ++c;
          pdst[c] = (pdst[c] * 10) + dec8(ch);
        }
        isok = isnum;
      }

      pdst[++c] = 0;   /* string padding character */
      dst->epos = c - 1;
      dst->size = c;
      return dst;
    }
  }

  return NULL;
}

handle_t dec32_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(srcsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      int c = -1;
      pulong_t pdst = CAST(pulong_t, dst->item);

      bool_t isok = FALSE;
      for (size_t i = 0; i < srcsize; ++i) {
        uchar_t ch = psrc[i];
        bool_t isnum = isdec8(ch);
        if (isnum) {
          if (!isok) ++c;
          pdst[c] = (pdst[c] * 10) + dec8(ch);
        }
        isok = isnum;
      }

      pdst[++c] = 0;   /* string padding character */
      dst->epos = c - 1;
      dst->size = c;
      return dst;
    }
  }

  return NULL;
}

handle_t hex_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(srcsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      dst->cpos = -1;
      puchar_t pdst = CAST(puchar_t, dst->item);

      size_t isok = 0;
      for (size_t i = 0; i < srcsize; ++i) {
        uchar_t ch0 = psrc[i];
        uchar_t ch1 = psrc[i + 1];
        if ('0' == ch0 && ('x' == ch1 || 'X' == ch1)) continue;
        bool_t ishex = ishex8(ch0);
        if (ishex) {
          if (0 == (isok++ & 0x01)) ++dst->cpos;
          pdst[dst->cpos] = (pdst[dst->cpos] << 4) + hex8(ch0);
        } else {
          isok = 0;
        }
      }

      pdst[++dst->cpos] = '\0';   /* string padding character */
      dst->size = dst->cpos;
      dst->epos = dst->cpos - 1;
      dst->cpos = 0;
      return dst;
    }
  }

  return NULL;
}

static uint8_t base32_decode0(char c) {
  if (c >= 'A' && c <= 'Z') {
    return c - 'A';
  } else if (c >= '2' && c <= '7') {
    return 26 + c - '2';
  }

  return 0xFF;
}

handle_t base32_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 3 / 4;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(maxsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);

      bool_t isok = FALSE;
      bool_t isdie = FALSE;
      for (size_t i = 0; i < srcsize && !isok && !isdie; ) {
        uint8_t tmp[8];

        size_t di = 8;
        for (size_t j = 0; j < 8; ) {
          uchar_t ch = psrc[i++];

          // Handle and validate padding.
          if (isspace(ch)) continue;
          else if ('\0' == ch) {
            di = j;
//            isdie = TRUE;
            break;
          }

          size_t sz = srcsize - i - 1;
          if (ch == '=' && j >= 2 && sz < 8) {
//        if (sz + j < 8 - 1) {
//          // not enough padding
//          return ECODE_CORRUPT;
//        }

//        for (int k = 0; k < 8 - 1 - i; k++) {
//          if (len > k && psrc[k] != '=') {
//            return ECODE_CORRUPT;
//          }
//        }

            di = j;
            isok = TRUE;

            if (1 == di || 3 == di || 6 == di) isdie = TRUE;

            break;
          }

          tmp[j++] = ch = base32_decode0(ch);
          if (ch == 0xFF) {
            isdie = TRUE;
            break;
          }
        }

        if (!isdie) {
          switch (di) {
          case 8:  pdst[dst->cpos + 4] = (tmp[6] << 5) |  tmp[7];
          case 7:  pdst[dst->cpos + 3] = (tmp[4] << 7) | (tmp[5] << 2) | (tmp[6] >> 3);
          case 5:  pdst[dst->cpos + 2] = (tmp[3] << 4) | (tmp[4] >> 1);
          case 4:  pdst[dst->cpos + 1] = (tmp[1] << 6) | (tmp[2] << 1) | (tmp[3] >> 4);
          case 2:  pdst[dst->cpos + 0] = (tmp[0] << 3) | (tmp[1] >> 2);
          }

          if (di < NELEMENTS(base32_ext)) {
            dst->cpos += base32_ext[di];
          }
        }
      }

      pdst[dst->cpos] = '\0';   /* string padding character */
      dst->size = dst->cpos;
      dst->epos = dst->cpos - 1;
      dst->cpos = 0;
      return dst;
    }
  }

  return NULL;
}

handle_t base58_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 3 / 4;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(maxsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);

      dst->cpos = 1;
      for (size_t i = 0; i < srcsize; ++i) {
        int32_t carry = base58_idx[psrc[i]];
        for (size_t j = 0; j < dst->cpos; ++j) {
          carry += CAST(int32_t, pdst[j]) * 58;
          pdst[j] = CAST(uchar_t, carry & 0xff);
          carry >>= 8;
        }
        while (carry > 0) {
          pdst[dst->cpos++] = (unsigned int)(carry & 0xff);
          carry >>= 8;
        }
      }

      for (size_t i = 0; i < srcsize && psrc[i] == '1'; ++i) {
        pdst[dst->cpos++] = 0;
      }

      for (size_t i = dst->cpos - 1, z = (dst->cpos >> 1) + (dst->cpos & 1); i >= z; i--) {
        uchar_t ch = pdst[i];
        pdst[i] = pdst[dst->cpos - i - 1];
        pdst[dst->cpos - i - 1] = ch;
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

handle_t base64_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 3 / 4;
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(maxsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);
      uchar_t tmp[4];
      size_t j = 0;
      for (size_t i = 0; i < srcsize; ++i) {
        uchar_t k;
        for (k = 0 ; k < 64 && base64_map[k] != psrc[i]; k++);

        tmp[j++] = k;
        if (j == 4) {
          pdst[dst->cpos++] = (tmp[0] << 2) + (tmp[1] >> 4);
          if (tmp[2] != 64)    pdst[dst->cpos++] = (tmp[1] << 4) + (tmp[2] >> 2);
          if (tmp[3] != 64)    pdst[dst->cpos++] = (tmp[2] << 6) + tmp[3];
          j = 0;
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

static bool_t base85_decode0(int c) {
    return (c < 33u) || (c > 117u);
}

handle_t base85_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(srcsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);

      size_t si = 0;
      bool_t isdie = FALSE;

      while (si < srcsize) {
        size_t chunk = 0u;
        size_t chunksize = srcsize - si;

        if (base85_decode_zero && ('z' == psrc[si])) {
          si += 1;
          chunk = 0u;
          chunksize = 5;
        } else if (base85_check_chars
               && (                      base85_decode0(psrc[si + 0])
                  || ((chunksize > 1) && base85_decode0(psrc[si + 1]))
                  || ((chunksize > 2) && base85_decode0(psrc[si + 2]))
                  || ((chunksize > 3) && base85_decode0(psrc[si + 3]))
                  || ((chunksize > 4) && base85_decode0(psrc[si + 4])))) {
            isdie = TRUE;
            break;
        }
        else if (chunksize >= 5) {
          chunk  = psrc[si++] - base85_def;
          chunk *= base85_max;
          chunk += psrc[si++] - base85_def;
          chunk *= base85_max;
          chunk += psrc[si++] - base85_def;
          chunk *= base85_max;
          chunk += psrc[si++] - base85_def;

          if (chunk > (UINT32_MAX / base85_max)) {
            isdie = TRUE;
            break;
          } else {
            uint8_t addend = psrc[si++] - base85_def;
            chunk *= base85_max;

            if (chunk > (UINT32_MAX - addend)) {
              isdie = TRUE;
              break;
            } else {
              chunk += addend;
            }
          }
        } else {
          chunk  = psrc[si++] - base85_def;
          chunk *= base85_max;
          chunk += ((si < srcsize) ? (psrc[si++] - base85_def) : base85_max - 1);
          chunk *= base85_max;
          chunk += ((si < srcsize) ? (psrc[si++] - base85_def) : base85_max - 1);
          chunk *= base85_max;
          chunk += ((si < srcsize) ? (psrc[si++] - base85_def) : base85_max - 1);

          if (chunk > (UINT32_MAX / base85_max)) {
            isdie = TRUE;
            break;
          } else  {
            uint8_t addend = (uint8_t )((si < srcsize) ? (psrc[si++] - base85_def) : base85_max - 1);
            chunk *= base85_max;

            if (chunk > (UINT32_MAX - addend)) {
              isdie = TRUE;
              break;
            } else {
              chunk += addend;
            }
          }
        }

        pdst[dst->cpos + 3] = (chunk % base85_maxchunk);
        chunk /= base85_maxchunk;
        pdst[dst->cpos + 2] = (chunk % base85_maxchunk);
        chunk /= base85_maxchunk;
        pdst[dst->cpos + 1] = (chunk % base85_maxchunk);
        chunk /= base85_maxchunk;
        pdst[dst->cpos + 0] = chunk;

        dst->cpos += chunksize >= 5 ? 4 : chunksize - 1;
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

int vigenere_code(int c) {
  if ('A' <= c && c <= 'Z') {
    return c - 'A';
  } else if ('a' <= c && c <= 'z') {
    return c - 'a';
  }

  return 0;
}

handle_t vigenere_decode(unknown_t src, size_t srcsize, unknown_t key, size_t keysize) {
  if (src && srcsize && key && keysize) {
    pfind_t dst = fxalloc(srcsize, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->item);

      puchar_t pkey = CAST(puchar_t, key);
      puchar_t psrc = CAST(puchar_t, src);

      size_t j = 0;
      for (size_t i = 0; i < srcsize; ++i) {
        if ('a' <= psrc[i] && psrc[i] <= 'z') {
          int x = (vigenere_code(psrc[i]) - vigenere_code(pkey[j++])) % 26;
          pdst[dst->cpos++] = x < 0 ? 'z' + x + 1: 'a' + x;
        } else if ('A' <= psrc[i] && psrc[i] <= 'Z') {
          int x = (vigenere_code(psrc[i]) - vigenere_code(pkey[j++])) % 26;
          pdst[dst->cpos++] = x < 0 ? 'Z' + x + 1: 'A' + x;
        } else {
          pdst[dst->cpos++] = psrc[i];
        }

        if (j >= keysize) j = 0;
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

uint64_t sleb128_decode(unknown_t src, size_t srcsize) {
  uint64_t n = 0;

  if (src && srcsize) {
    uchar_t x = 0;
    uint64_t y = 0;
    puchar_t psrc = CAST(puchar_t, src);
    for (size_t i = 0; i < srcsize; ++i) {
      x = psrc[i];
      n |= (x & 0x7f) << y;
      y += 7;

      if ((x & 0x80) == 0) break;
    }

    if ((y < 64) && (x & 0x40)) {
      n |= -(1 << y);
    }
  }

  return n;
}

uint64_t uleb128_decode(unknown_t src, size_t srcsize) {
  uint64_t n = 0;

  if (src && srcsize) {
    uint64_t y = 0;
    puchar_t psrc = CAST(puchar_t, src);
    for (size_t i = 0; i < srcsize; ++i) {
      uchar_t x = psrc[i];
      n |= (x & 0x7f) << y;
      y += 7;
      if ((x & 0x80) == 0) break;
    }
  }

  return n;
}

