#include <math.h>
#include <fuzzy.h>
#include <openssl/evp.h>
#include "hash.h"
#include "memuse.h"
#include "memfind.h"

static int evp(const EVP_MD* q, const unknown_t p, const size_t size, puchar_t md) {
  if (NULL == p || NULL == q) return -1;

  unknown_t context = EVP_MD_CTX_create();
  if (context) {
    int x = EVP_DigestInit_ex(context, q, NULL);
    if (x) {
      x = EVP_DigestUpdate(context, p, size);
    }

    if (x) {
      x = EVP_DigestFinal_ex(context, md, NULL);
    }

    EVP_MD_CTX_destroy(context);

    return x ? 0 : -1;
  }

  return -1;
}

int md5(const unknown_t p, const size_t size, puchar_t md) {
  xmemclr(md, MD5_DIGEST_LENGTH);
#ifdef BUILD_UBUNTU_24_04
  return evp(EVP_md5(), p, size, md);
#else
  MD5_CTX context;

  if (NULL == p)                             return -1;
  if (!MD5_Init(&context))                   return -1;
  if (!MD5_Update(&context, p, size))        return -1;
  if (!MD5_Final(md, &context))              return -1;

  return 0;
#endif
}

int sha1(const unknown_t p, const size_t size, puchar_t md) {
  xmemclr(md, SHA_DIGEST_LENGTH);
#ifdef BUILD_UBUNTU_24_04
  return evp(EVP_sha1(), p, size, md);
#else
  SHA_CTX context;

  if (NULL == p)                             return -1;
  if (!SHA1_Init(&context))                  return -1;
  if (!SHA1_Update(&context, p, size))       return -1;
  if (!SHA1_Final(md, &context))             return -1;

  return 0;
#endif
}

int sha256(const unknown_t p, const size_t size, puchar_t md) {
  xmemclr(md, SHA256_DIGEST_LENGTH);
#ifdef BUILD_UBUNTU_24_04
  return evp(EVP_sha256(), p, size, md);
#else
  SHA256_CTX context;

  if (NULL == p)                             return -1;
  if (!SHA256_Init(&context))                return -1;
  if (!SHA256_Update(&context, p, size))     return -1;
  if (!SHA256_Final(md, &context))           return -1;

  return 0;
#endif
}

int sha512(const unknown_t p, const size_t size, puchar_t md) {
  xmemclr(md, SHA512_DIGEST_LENGTH);
#ifdef BUILD_UBUNTU_24_04
  return evp(EVP_sha512(), p, size, md);
#else
  SHA512_CTX context;
  if (NULL == p)                             return -1;
  if (!SHA512_Init(&context))                return -1;
  if (!SHA512_Update(&context, p, size))     return -1;
  if (!SHA512_Final(md, &context))           return -1;

  return 0;
#endif
}

int ripemd160(const unknown_t p, const size_t size, puchar_t md) {
  xmemclr(md, RIPEMD160_DIGEST_LENGTH);
#ifdef BUILD_UBUNTU_24_04
  return evp(EVP_ripemd160(), p, size, md);
#else
  RIPEMD160_CTX context;

  if (NULL == p)                             return -1;
  if (!RIPEMD160_Init(&context))             return -1;
  if (!RIPEMD160_Update(&context, p, size))  return -1;
  if (!RIPEMD160_Final(md, &context))        return -1;

  return 0;
#endif
}

// Shannon Entropy
int entropy_calculate(const unknown_t p, const size_t size, double *entropy, double *min, double *max) {
  MALLOCA(int32_t, counts, 256);
  if (min) *min = 0.0;
  if (max) *max = log2(NELEMENTS(counts));
  if (entropy) *entropy = 0.0;

  if (p && 0 != size && entropy && min && max) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      counts[p0[i]]++;
    }

    for (size_t i = 0; i < NELEMENTS(counts); ++i) {
      if (counts[i]) {
        double probability = CAST(double, counts[i]) / CAST(double, size);
        *entropy -= probability * log2(probability);
      }
    }

    return 0;
  }

  return -1;
}

handle_t ssdeep_encode(unknown_t src, size_t srcsize) {
  if (src && srcsize) {
    puchar_t psrc = CAST(puchar_t, src);

    pfind_t dst = fxalloc(FUZZY_MAX_RESULT, MEMFIND_NOCHUNKSIZE);
    if (dst) {
      char *pdst = CAST(char *, dst->item);
      if (!fuzzy_hash_buf(psrc, srcsize, pdst)) {
        dst->cpos = xstrnlen(pdst, FUZZY_MAX_RESULT);
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

