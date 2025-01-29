#include <inttypes.h>

#include "printf.h"

int printf_nice(const uint64_t value, const int mode) {
  switch (mode) {
  case USE_DEC:                     return printf(" %" PRId64, value);
  case USE_DEC2:                    return printf(" %2" PRId64, value);
  case USE_FHEX:                    return printf(" 0x%" PRIx64, value);
  case USE_LHEX:                    return printf(" %" PRIx64, value);
  case USE_FHEX8:                   return printf(" 0x%2.2" PRIx64, value);
  case USE_LHEX8:                   return printf(" %2.2" PRIx64, value);
  case USE_FHEX16:                  return printf(" 0x%4.4" PRIx64, value);
  case USE_LHEX16:                  return printf(" %4.4" PRIx64, value);
  case USE_FHEX32:                  return printf(" 0x%8.8" PRIx64, value);
  case USE_LHEX32:                  return printf(" %8.8" PRIx64, value);
  case USE_FHEX64:                  return printf(" 0x%16.16" PRIx64, value);
  case USE_LHEX64:                  return printf(" %16.16" PRIx64, value);
  case USE_CHAR:                    return printf("%c", (int)(value & 0xff));
  default:
    break;
  }

  return 0;
}

int printf_data(const void* data, const size_t size, const int mode) {
#define MAX_SIZE (16)
  const unsigned char *p = data;
  for (size_t i = 0; i < size; ) {
    size_t siz = size > MAX_SIZE ? MAX_SIZE : size;
    for (size_t j = 0; j < MAX_SIZE; j++) {
      if (j < siz)  printf_nice(p[j], USE_LHEX8);
      else printf("  ");
    }

    printf("  ");

    for (size_t k = 0; k < siz; ++k) {
      char c = p[k];
      if (c >= ' ' && c < 0x7f) printf_nice(c, USE_CHAR);
      else printf_nice('.', USE_CHAR);
    }

    printf_nice('\n', USE_CHAR);
    p += siz;
    i += siz;
  }
#undef MAX_SIZE
  return 0;
}
