#include <ctype.h>
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
  case USE_FHEX24:                  return printf(" 0x%6.6" PRIx64, value);
  case USE_LHEX24:                  return printf(" %6.6" PRIx64, value);
  case USE_FHEX32:                  return printf(" 0x%8.8" PRIx64, value);
  case USE_LHEX32:                  return printf(" %8.8" PRIx64, value);
  case USE_FHEX48:                  return printf(" 0x%12.12" PRIx64, value);
  case USE_LHEX48:                  return printf(" %12.12" PRIx64, value);
  case USE_FHEX64:                  return printf(" 0x%16.16" PRIx64, value);
  case USE_LHEX64:                  return printf(" %16.16" PRIx64, value);
  default:
    break;
  }

  return -1;
}

int printf_data(const void* data, const size_t size, const int addr, const int mode) {
#define MAX_SIZE (16)
  int x = addr;
  size_t i = 0;
  const unsigned char *p = data;
  for (i = 0; i < size; ) {
    if (USE_HEXDUMP == mode) {
      printf_nice(x, USE_FHEX32);

      size_t siz = (size - i) > MAX_SIZE ? MAX_SIZE : (size - i);
      for (size_t j = 0; j < MAX_SIZE; j++) {
        if (j < siz) printf_nice(p[j], USE_LHEX8);
        else printf("   ");
      }

      printf("  ");

      for (size_t k = 0; k < siz; ++k) {
        printf("%c", isprint(p[k]) ? p[k] : '.');
      }

      printf("\n");

      p += siz;
      i += siz;
      x += siz;
    } else if (USE_STRDUMP == mode) {
      while (!isprint(*p) && i < size) {
        ++p;
        ++i;
      }

      printf ("  [%6lx]  ", i);

      while (0 != *p && i < size) {
        if (iscntrl(*p)) printf("^%c", *p + 0x40);
        else if (isprint(*p)) printf("%c", *p);
        ++p;
        ++i;
      }
      ++p;
      ++i;

      printf("\n");
    } else if (USE_STR == mode) {
      if (0 == *p) return i;
      else if (iscntrl(*p)) printf("^%c", *p + 0x40);
      else printf("%c", isprint(*p) ? *p : '.');
      ++p;
      ++i;
    } else if (USE_HEX == mode) {
      printf("%2x", *p);
      ++p;
      ++i;
    } else {
      return -1;
    }
  }
#undef MAX_SIZE
  return i;
}

