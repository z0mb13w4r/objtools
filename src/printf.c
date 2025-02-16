#include <time.h>
#include <ctype.h>
#include <inttypes.h>

#include "printf.h"

int printf_eol() {
  return printf("\n");
}

int printf_nice(const uint64_t v, const int mode) {
  switch (mode) {
  case USE_DEC:                     return printf(" %" PRId64, v);
  case USE_DEC2:                    return printf(" %2" PRId64, v);
  case USE_DEC5:                    return printf(" %5" PRId64, v);
  case USE_OCT:                     return printf(" %" PRIo64, v);
  case USE_OCT2:                    return printf(" %2" PRIo64, v);
  case USE_OCT5:                    return printf(" %5" PRIo64, v);
  case USE_FHEX:                    return printf(" 0x%" PRIx64, v);
  case USE_LHEX:                    return printf(" %" PRIx64, v);
  case USE_FHEX8:                   return printf(" 0x%2.2" PRIx64, v);
  case USE_LHEX8:                   return printf(" %2.2" PRIx64, v);
  case USE_FHEX16:                  return printf(" 0x%4.4" PRIx64, v);
  case USE_LHEX16:                  return printf(" %4.4" PRIx64, v);
  case USE_FHEX24:                  return printf(" 0x%6.6" PRIx64, v);
  case USE_LHEX24:                  return printf(" %6.6" PRIx64, v);
  case USE_FHEX32:                  return printf(" 0x%8.8" PRIx64, v);
  case USE_LHEX32:                  return printf(" %8.8" PRIx64, v);
  case USE_FHEX48:                  return printf(" 0x%12.12" PRIx64, v);
  case USE_LHEX48:                  return printf(" %12.12" PRIx64, v);
  case USE_FHEX64:                  return printf(" 0x%16.16" PRIx64, v);
  case USE_LHEX64:                  return printf(" %16.16" PRIx64, v);
  case USE_CORRUPT:                 return printf(" <corrupt: %" PRIx64 ">", v);
  case USE_UNKNOWN:                 return printf(" <unknown: %" PRIx64 ">", v);

  case USE_CHARCTRL:
    if (iscntrl(v))                 return printf("^%c", CAST(int, v) + 0x40);
    if (isprint(v))                 return printf("%c", CAST(int, v));
    return printf(".");

  case USE_TIMEDATE: {
    struct tm * tmp;
    time_t atime = v;

    tmp = gmtime (&atime);
    if (tmp == NULL)                return printf_nice(atime, USE_CORRUPT);

    return printf("%04u-%02u-%02uT%02u:%02u:%02u",
                   tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
                   tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
    }
    break;

  default:
    break;
  }

  return -1;
}

int printf_text(const char* p, const int mode) {
  if (p) {
    int n = 0;

    switch (GET_PREFIX(mode)) {
    case USE_SPACE:          n += printf(" ");         break;
    default:                 break;
    }

    switch (GET_BRACKET(mode)) {
    case USE_RB:             n += printf("(%s)", p);   break;
    case USE_SB:             n += printf("[%s]", p);   break;
    case USE_SQ:             n += printf("'%s'", p);   break;
    case USE_DQ:             n += printf("\"%s\"", p); break;
    default:                 break;
    }

    if (0 == GET_BRACKET(mode)) {
      switch (GET_FORMAT(mode)) {
      case USE_NONE:
      case USE_LT:           n += printf("%s", p);     break;
      default:               break;
      }
    }

    switch (GET_POSTFIX(mode)) {
    case USE_COLON:          n += printf(":");         break;
    default:                 break;
    }

    if (mode & USE_EOL)      n += printf_eol();

    return n;
  }

  return -1;
}

int printf_data(const void* p, const size_t size, const int addr, const int mode) {
#define MAX_SIZE (16)
  int x = addr;
  size_t i = 0;
  const unsigned char *pp = p;
  for (i = 0; i < size; ) {
    if (USE_HEXDUMP == mode) {
      printf_nice(x, USE_FHEX32);

      size_t siz = (size - i) > MAX_SIZE ? MAX_SIZE : (size - i);
      for (size_t j = 0; j < MAX_SIZE; j++) {
        if (j < siz) printf_nice(pp[j], USE_LHEX8);
        else printf("   ");
      }

      printf("  ");

      for (size_t k = 0; k < siz; ++k) {
        printf("%c", isprint(pp[k]) ? pp[k] : '.');
      }

      printf_eol();

      pp += siz;
      i += siz;
      x += siz;
    } else if (USE_STRDUMP == mode) {
      while (!isprint(*pp) && i < size) {
        ++pp;
        ++i;
      }

      printf ("  [%6lx]  ", i);

      while (0 != *pp && i < size) {
        printf_nice(*pp, USE_CHARCTRL);
        ++pp;
        ++i;
      }
      ++pp;
      ++i;

      printf("\n");
    } else if (USE_STR == mode) {
      if (0 == *pp) return i;
      printf_nice(*pp, USE_CHARCTRL);
      ++pp;
      ++i;
    } else if (USE_HEX == mode) {
      printf_nice(*pp, USE_LHEX8);
      ++pp;
      ++i;
    } else {
      return -1;
    }
  }
#undef MAX_SIZE
  return i;
}

int printf_mask(const pconvert_t p, const unsigned int mask) {
  int n = 0;
  unsigned int v = mask;
  for (pconvert_t x = p; 0 != x->text; ++x) {
    if (x->type & v) {
      n += printf(" %s", x->text);
      v &= ~x->type;
    }
  }

  if (v) {
    n += printf_nice(v, USE_UNKNOWN);
  }

  return n;
}

int printf_masknone(const pconvert_t p, const unsigned int mask) {
  if (0 == mask) {
    return printf(" NONE");
  }

  return printf_mask(p, mask);
}

