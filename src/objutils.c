#include <stdio.h>
#include <string.h>

#include "objutils.h"

bool_t isbits(const imode_t x) {
  return x & (x -1);
}

bool_t isused(pick_t* p, const pick_t x) {
  if (p) {
    for ( ; PICK_END != *p; ++p) {
      if (x == *p) return TRUE;
    }
  }

  return FALSE;
}

uint64_t ulog2(uint64_t  x) {
  uint64_t y = 0;

  if (x > 1) {
    --x;
    do {
      ++y;
    } while (0 != (x >>= 1));
  }

  return y;
}

char* strname(char* dst, const char* src) {
  char *p = strrchr(src, '/');
  if (p) {
    return strcpy(dst, ++p);
  }

  return strcpy(dst, src);
}

const char* strpick(const pconvert_t p, const pick_t x) {
  const char* s = strpicknull(p, x);
  if (NULL == s) {
    return strpickunknown(x);
  }

  return s;
}

const char* strpicknull(const pconvert_t p, const pick_t x) {
  for (pconvert_t pp = p; 0 != pp->text; ++pp) {
    if (pp->type == x) {
      return pp->text;
    }
  }

  return NULL;
}

const char* strpickunknown(const pick_t x) {
  static char o[32];
  snprintf(o, sizeof(o), "<unknown: %x>", x);
  return o;
}

size_t strlenpick(const pconvert_t p) {
  size_t siz = 0;

  for (pconvert_t pp = p; 0 != pp->text; ++pp) {
    siz = MAX(siz, strlen(pp->text));
  }

  return siz;
}

size_t fsize(FILE *f) {
  size_t siz = 0;
  if (f) {
    fseek(f, 0, SEEK_END);
    siz = ftell(f);
    fseek(f, 0, SEEK_SET);
  }
  return siz;
}

int md5(const unknown_t p, const size_t size, puchar_t md) {
  MD5_CTX context;
  if (!MD5_Init(&context))
    return -1;

  if (!MD5_Update(&context, p, size))
    return -1;

  if (!MD5_Final(md, &context))
    return -1;

  return 0;
}

int sha1(const unknown_t p, const size_t size, puchar_t md) {
  SHA_CTX context;
  if (!SHA1_Init(&context))
    return -1;

  if (!SHA1_Update(&context, p, size))
    return -1;

  if (!SHA1_Final(md, &context))
    return -1;

  return 0;
}

int sha256(const unknown_t p, const size_t size, puchar_t md) {
  SHA256_CTX context;
  if (!SHA256_Init(&context))
    return -1;

  if (!SHA256_Update(&context, p, size))
    return -1;

  if (!SHA256_Final(md, &context))
    return -1;

  return 0;
}

int sha512(const unknown_t p, const size_t size, puchar_t md) {
  SHA512_CTX context;
  if (!SHA512_Init(&context))
    return -1;

  if (!SHA512_Update(&context, p, size))
    return -1;

  if (!SHA512_Final(md, &context))
    return -1;

  return 0;
}

int ripemd160(const unknown_t p, const size_t size, puchar_t md) {
  RIPEMD160_CTX context;
  if (!RIPEMD160_Init(&context))
    return -1;

  if (!RIPEMD160_Update(&context, p, size))
    return -1;

  if (!RIPEMD160_Final(md, &context))
    return -1;

  return 0;
}

int crc32(const unknown_t p, const size_t size, pulong_t crc) {

  return 0;
}
