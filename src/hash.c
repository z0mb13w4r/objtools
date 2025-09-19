#include <math.h>
#include "hash.h"
#include "memuse.h"

int md5(const unknown_t p, const size_t size, puchar_t md) {
  MD5_CTX context;
  if (NULL == p)                             return -1;
  if (!MD5_Init(&context))                   return -1;
  if (!MD5_Update(&context, p, size))        return -1;
  if (!MD5_Final(md, &context))              return -1;

  return 0;
}

int sha1(const unknown_t p, const size_t size, puchar_t md) {
  SHA_CTX context;
  xmemclr(md, SHA_DIGEST_LENGTH);

  if (NULL == p)                             return -1;
  if (!SHA1_Init(&context))                  return -1;
  if (!SHA1_Update(&context, p, size))       return -1;
  if (!SHA1_Final(md, &context))             return -1;

  return 0;
}

int sha256(const unknown_t p, const size_t size, puchar_t md) {
  SHA256_CTX context;
  xmemclr(md, SHA256_DIGEST_LENGTH);

  if (NULL == p)                             return -1;
  if (!SHA256_Init(&context))                return -1;
  if (!SHA256_Update(&context, p, size))     return -1;
  if (!SHA256_Final(md, &context))           return -1;

  return 0;
}

int sha512(const unknown_t p, const size_t size, puchar_t md) {
  SHA512_CTX context;
  xmemclr(md, SHA512_DIGEST_LENGTH);

  if (NULL == p)                             return -1;
  if (!SHA512_Init(&context))                return -1;
  if (!SHA512_Update(&context, p, size))     return -1;
  if (!SHA512_Final(md, &context))           return -1;

  return 0;
}

int ripemd160(const unknown_t p, const size_t size, puchar_t md) {
  RIPEMD160_CTX context;
  xmemclr(md, RIPEMD160_DIGEST_LENGTH);

  if (NULL == p)                             return -1;
  if (!RIPEMD160_Init(&context))             return -1;
  if (!RIPEMD160_Update(&context, p, size))  return -1;
  if (!RIPEMD160_Final(md, &context))        return -1;

  return 0;
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

