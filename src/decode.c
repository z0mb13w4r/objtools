#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
//#include <openssl/err.h>

#include "decode.h"

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

    return 0;
  }

  return -1;
}

int rot13(const unknown_t p, const size_t size) {
  if (p) {
    char *p0 = CAST(char*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = zap(p0[i], 'A', 'Z', 13);
      p0[i] = zap(p0[i], 'a', 'z', 13);
    }

    return 0;
  }

  return -1;
}

int rot18(const unknown_t p, const size_t size) {
  if (p) {
    char *p0 = CAST(char*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = zap(p0[i], 'A', 'Z', 13);
      p0[i] = zap(p0[i], 'a', 'z', 13);
      p0[i] = zap(p0[i], '0', '9', 5);
    }

    return 0;
  }

  return -1;
}

int xor8(const unknown_t p, const uint8_t v, const size_t size) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] ^= v;
    }

    return 0;
  }

  return -1;
}

int xor16(const unknown_t p, const uint16_t v, const size_t size) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      p0[i] ^= v;
    }

    return 0;
  }

  return -1;
}

int xor32(const unknown_t p, const uint32_t v, const size_t size) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      p0[i] ^= v;
    }

    return 0;
  }

  return -1;
}

int add8(const unknown_t p, const uint8_t v, const size_t size) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    for (size_t i = 0; i < size; ++i) {
      const int32_t v0 = p0[i] + v;
      p0[i] = v0 > UCHAR_MAX ? v0 - UCHAR_MAX : v0;
    }

    return 0;
  }

  return -1;
}

int add16(const unknown_t p, const uint16_t v, const size_t size) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      const int32_t v0 = p0[i] + v;
      p0[i] = v0 > USHRT_MAX ? v0 - USHRT_MAX : v0;
    }

    return 0;
  }

  return -1;
}

int add32(const unknown_t p, const uint32_t v, const size_t size) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      const int64_t v0 = p0[i] + v;
      p0[i] = v0 > ULONG_MAX ? v0 - ULONG_MAX : v0;
    }

    return 0;
  }

  return -1;
}

int sub8(const unknown_t p, const uint8_t v, const size_t size) {
  if (p) {
    uint8_t *p0 = CAST(uint8_t*, p);
    for (size_t i = 0; i < size; ++i) {
      const int32_t v0 = p0[i] - v;
      p0[i] = v0 < 0 ? UCHAR_MAX + v0 : v0;
    }

    return 0;
  }

  return -1;
}

int sub16(const unknown_t p, const uint16_t v, const size_t size) {
  if (p) {
    uint16_t *p0 = CAST(uint16_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint16_t)); ++i) {
      const int32_t v0 = p0[i] - v;
      p0[i] = v0 < 0 ? USHRT_MAX + v0 : v0;
    }

    return 0;
  }

  return -1;
}

int sub32(const unknown_t p, const uint32_t v, const size_t size) {
  if (p) {
    uint32_t *p0 = CAST(uint32_t*, p);
    for (size_t i = 0; i < (size / sizeof(uint32_t)); ++i) {
      const int64_t v0 = p0[i] - v;
      p0[i] = v0 < 0 ? ULONG_MAX + v0 : v0;
    }

    return 0;
  }

  return -1;
}

int aes_encrypt(const int mode, puchar_t src, const size_t srcsize,
                puchar_t key, const size_t keysize, puchar_t ivec, puchar_t dst) {

//   ERR_load_crypto_strings();
//   OpenSSL_add_all_algorithms();

  int size = 0;
  int dstsize = 0;

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (NULL == ctx) return -1;

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

