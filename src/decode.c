#include <ctype.h>
#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
//#include <openssl/err.h>

#include "buffer.h"
#include "decode.h"
#include "bstring.h"

uchar_t base64_map[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
  'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

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

int shl8(const unknown_t p, const uint8_t v, const size_t size) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] <<= v;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int shl16(const unknown_t p, const uint16_t v, const size_t size) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      p0[i] <<= v;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int shl32(const unknown_t p, const uint32_t v, const size_t size) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      p0[i] <<= v;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int shr8(const unknown_t p, const uint8_t v, const size_t size) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] >>= v;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int shr16(const unknown_t p, const uint16_t v, const size_t size) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      p0[i] >>= v;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int shr32(const unknown_t p, const uint32_t v, const size_t size) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      p0[i] >>= v;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int rol8(const unknown_t p, const uint8_t v, const size_t size) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = (p0[i] << v) | (p0[i] >> (8 - v));
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int rol16(const unknown_t p, const uint16_t v, const size_t size) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      p0[i] = (p0[i] << v) | (p0[i] >> (16 - v));
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int rol32(const unknown_t p, const uint32_t v, const size_t size) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      p0[i] = (p0[i] << v) | (p0[i] >> (32 - v));
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int ror8(const unknown_t p, const uint8_t v, const size_t size) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = (p0[i] >> v) | (p0[i] << (8 - v));
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int ror16(const unknown_t p, const uint16_t v, const size_t size) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      p0[i] = (p0[i] >> v) | (p0[i] << (16 - v));
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int ror32(const unknown_t p, const uint32_t v, const size_t size) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      p0[i] = (p0[i] >> v) | (p0[i] << (32 - v));
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

handle_t dec8_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    puchar_t psrc = CAST(puchar_t, src);

    pbstring_t dst = bstrmallocsize(srcsize);
    if (dst) {
      int c = -1;
      puchar_t pdst = CAST(puchar_t, dst->data);

      bool_t isok = FALSE;
      for (size_t i = 0; i < srcsize; ++i) {
        uchar_t ch = psrc[i];
        bool_t isnum = isdigit(ch);
        if (isnum) {
          if (!isok) ++c;
          pdst[c] = (pdst[c] * 10) + (ch - '0');
        }
        isok = isnum;
      }

      pdst[++c] = '\0';   /* string padding character */
      dst->size = c;
      return dst;
    }
  }

  return NULL;
}

handle_t dec16_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    puchar_t psrc = CAST(puchar_t, src);

    pbstring_t dst = bstrmallocsize(srcsize);
    if (dst) {
      int c = -1;
      pushort_t pdst = CAST(pushort_t, dst->data);

      bool_t isok = FALSE;
      for (size_t i = 0; i < srcsize; ++i) {
        uchar_t ch = psrc[i];
        bool_t isnum = isdigit(ch);
        if (isnum) {
          if (!isok) ++c;
          pdst[c] = (pdst[c] * 10) + (ch - '0');
        }
        isok = isnum;
      }

      pdst[++c] = '\0';   /* string padding character */
      dst->size = c;
      return dst;
    }
  }

  return NULL;
}

handle_t dec32_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    puchar_t psrc = CAST(puchar_t, src);

    pbstring_t dst = bstrmallocsize(srcsize);
    if (dst) {
      int c = -1;
      pulong_t pdst = CAST(pulong_t, dst->data);

      bool_t isok = FALSE;
      for (size_t i = 0; i < srcsize; ++i) {
        uchar_t ch = psrc[i];
        bool_t isnum = isdigit(ch);
        if (isnum) {
          if (!isok) ++c;
          pdst[c] = (pdst[c] * 10) + (ch - '0');
        }
        isok = isnum;
      }

      pdst[++c] = '\0';   /* string padding character */
      dst->size = c;
      return dst;
    }
  }

  return NULL;
}

handle_t base64_decode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    size_t maxsize = srcsize * 3 / 4;
    puchar_t psrc = CAST(puchar_t, src);

    pbstring_t dst = bstrmallocsize(maxsize);
    if (dst) {
      puchar_t pdst = CAST(puchar_t, dst->data);
      uchar_t tmp[4];
      int p = 0, j = 0;
      for (int i = 0; i < srcsize; ++i) {
        uchar_t k;
        for (k = 0 ; k < 64 && base64_map[k] != psrc[i]; k++);

        tmp[j++] = k;
        if (j == 4) {
          pdst[p++] = (tmp[0] << 2) + (tmp[1] >> 4);
          if (tmp[2] != 64)    pdst[p++] = (tmp[1] << 4) + (tmp[2] >> 2);
          if (tmp[3] != 64)    pdst[p++] = (tmp[2] << 6) + tmp[3];
          j = 0;
        }
      }

      pdst[p] = '\0';   /* string padding character */
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

