#include "hash.h"

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
  if (NULL == p)                             return -1;
  if (!SHA1_Init(&context))                  return -1;
  if (!SHA1_Update(&context, p, size))       return -1;
  if (!SHA1_Final(md, &context))             return -1;

  return 0;
}

int sha256(const unknown_t p, const size_t size, puchar_t md) {
  SHA256_CTX context;
  if (NULL == p)                             return -1;
  if (!SHA256_Init(&context))                return -1;
  if (!SHA256_Update(&context, p, size))     return -1;
  if (!SHA256_Final(md, &context))           return -1;

  return 0;
}

int sha512(const unknown_t p, const size_t size, puchar_t md) {
  SHA512_CTX context;
  if (NULL == p)                             return -1;
  if (!SHA512_Init(&context))                return -1;
  if (!SHA512_Update(&context, p, size))     return -1;
  if (!SHA512_Final(md, &context))           return -1;

  return 0;
}

int ripemd160(const unknown_t p, const size_t size, puchar_t md) {
  RIPEMD160_CTX context;
  if (NULL == p)                             return -1;
  if (!RIPEMD160_Init(&context))             return -1;
  if (!RIPEMD160_Update(&context, p, size))  return -1;
  if (!RIPEMD160_Final(md, &context))        return -1;

  return 0;
}

