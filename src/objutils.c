#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

#include "objutils.h"

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
  static char buff[32];
  snprintf(buff, sizeof (buff), "<unknown: %x>", x);
  return buff;
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

int sha256(const void* p, const size_t size, unsigned char* md) {
  SHA256_CTX context;
  if (!SHA256_Init(&context))
    return -1;

  if (!SHA256_Update(&context, p, size))
    return -1;

  if (!SHA256_Final(md, &context))
    return -1;

  return 0;
}

int sha512(const void* p, const size_t size, unsigned char* md) {
  SHA512_CTX context;
  if (!SHA512_Init(&context))
    return -1;

  if (!SHA512_Update(&context, p, size))
    return -1;

  if (!SHA512_Final(md, &context))
    return -1;

  return 0;
}

