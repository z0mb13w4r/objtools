#include <time.h>
#include <ctype.h>
#include <inttypes.h>

#include "printf.h"

int printf_eol() {
  return printf("\n");
}

int printf_nice(const uint64_t v, const modez_t mode) {
  int n = 0;

  switch (GET_POS0(mode)) {
  case USE_TAB:                  n += printf(" ");     break;
  default:                       break;
  }

  const modez_t xmode = mode & ~(USE_FLAGMASK | USE_POS0MASK | USE_POS1MASK | USE_BRACKETMASK);
  const int usespace = USE_CHARCTRL != xmode && USE_LHEX8NS != xmode;

  switch (GET_BRACKET(mode)) {
  case USE_CB:                   n += printf(" {");    break;
  case USE_RB:                   n += printf(" (");    break;
  case USE_SB:                   n += printf(" [");    break;
  case USE_TB:                   n += printf(" <");    break;
  case USE_DRTB:                 n += printf( ">>");   break;
  case USE_SQ:                   n += printf(" '");    break;
  case USE_DQ:                   n += printf(" \"");   break;
  default:
    if (usespace)                n += printf(" ");
    break;
  }

  switch (xmode) {
  case USE_DEC:                  n += printf("%" PRId64, v);                     break;
  case USE_DEC2:                 n += printf("%2" PRId64, v);                    break;
  case USE_DEC2Z:                n += printf("%2.2" PRId64, v);                  break;
  case USE_DEC3:                 n += printf("%3" PRId64, v);                    break;
  case USE_DEC3Z:                n += printf("%3.3" PRId64, v);                  break;
  case USE_DEC5:                 n += printf("%5" PRId64, v);                    break;
  case USE_DEC5Z:                n += printf("%5.5" PRId64, v);                  break;
  case USE_OCT:                  n += printf("%" PRIo64, v);                     break;
  case USE_OCT2:                 n += printf("%2" PRIo64, v);                    break;
  case USE_OCT5:                 n += printf("%5" PRIo64, v);                    break;
  case USE_FHEX:                 n += printf("0x%" PRIx64, v);                   break;
  case USE_LHEX:                 n += printf("%" PRIx64, v);                     break;
  case USE_FHEX8:                n += printf("0x%2.2" PRIx64, v);                break;
  case USE_LHEX8:                n += printf("%2.2" PRIx64, v);                  break;
  case USE_LHEX8NS:              n += printf("%2.2" PRIx64, v);                  break;
  case USE_FHEX16:               n += printf("0x%4.4" PRIx64, v);                break;
  case USE_LHEX16:               n += printf("%4.4" PRIx64, v);                  break;
  case USE_FHEX24:               n += printf("0x%6.6" PRIx64, v);                break;
  case USE_LHEX24:               n += printf("%6.6" PRIx64, v);                  break;
  case USE_FHEX32:               n += printf("0x%8.8" PRIx64, v);                break;
  case USE_LHEX32:               n += printf("%8.8" PRIx64, v);                  break;
  case USE_FHEX48:               n += printf("0x%12.12" PRIx64, v);              break;
  case USE_LHEX48:               n += printf("%12.12" PRIx64, v);                break;
  case USE_FHEX64:               n += printf("0x%16.16" PRIx64, v);              break;
  case USE_LHEX64:               n += printf("%16.16" PRIx64, v);                break;
  case USE_PERCENT:              n += printf("%3.1f%%", CAST(double, v) / 10);   break;
  case USE_ERROR:                n += printf("<error: %" PRIx64 ">", v);         break;
  case USE_CORRUPT:              n += printf("<corrupt: %" PRIx64 ">", v);       break;
  case USE_UNKNOWN:              n += printf("<unknown: %" PRIx64 ">", v);       break;
  case USE_WARNING:              n += printf("<warning: %" PRIx64 ">", v);       break;

  case USE_CHARCTRL:
    if (iscntrl(v))              n += printf("^%c", CAST(int, v) + 0x40);
    else if (isprint(v))         n += printf("%c", CAST(int, v));
    else                         n += printf(".");
    break;

  case USE_TIMEDATE: {
    struct tm * tmp;
    time_t atime = v;

    tmp = gmtime (&atime);
    if (tmp == NULL)             return printf_nice(atime, USE_CORRUPT);

    return printf("%04u-%02u-%02uT%02u:%02u:%02u",
                   tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
                   tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
    }
    break;

  default:
    break;
  }

  switch (GET_BRACKET(mode)) {
  case USE_CB:                   n += printf("}");         break;
  case USE_RB:                   n += printf(")");         break;
  case USE_SB:                   n += printf("]");         break;
  case USE_TB:                   n += printf(">");         break;
  case USE_DRTB:                 n += printf("<<");        break;
  case USE_SQ:                   n += printf("'");         break;
  case USE_DQ:                   n += printf("\"");        break;
  default:                       break;
  }

  switch (GET_POS1(mode)) {
  case USE_COLON:                n += printf(":");         break;
  case USE_BYTES:                n += printf(" (bytes)");  break;
  default:                       break;
  }

  if (mode & USE_EOL)            n += printf_eol();

  return n;
}

int printf_text(const char* p, const modez_t mode) {
  MALLOCA(char, data, 1024);
#define PRINT1(x)    snprintf(data + n, sizeof(data) - n, x)
#define PRINT2(x,y)  snprintf(data + n, sizeof(data) - n, x, y)
  if (p) {
    int n = 0;
    int b = GET_BRACKET(mode);
    int e = mode & USE_EOL;
    int p0 = GET_POS0(mode);
    int p1 = GET_POS1(mode);
    int ss = CAST(int, GET_PAD(mode));
    int rt = USE_RT == GET_FORMAT(mode);

    switch (p0) {
    case USE_AT:             n += PRINT1("@");         break;
    case USE_SPACE:          n += PRINT1(" ");         break;
    case USE_TAB:            n += PRINT1("  ");        break;
    default:                 break;
    }

    switch (b) {
    case USE_CB:             n += PRINT2("{%s}", p);   break;
    case USE_RB:             n += PRINT2("(%s)", p);   break;
    case USE_SB:             n += PRINT2("[%s]", p);   break;
    case USE_TB:             n += PRINT2("<%s>", p);   break;
    case USE_DRTB:           n += PRINT2(">>%s<<", p); break;
    case USE_SQ:             n += PRINT2("'%s'", p);   break;
    case USE_DQ:             n += PRINT2("\"%s\"", p); break;
    default:                 n += PRINT2("%s", p);     break;
    }

    switch (p1) {
    case USE_COLON:          n += PRINT1(":");         break;
    case USE_BYTES:          n += PRINT1(" (bytes)");  break;
    default:                 break;
    }

    if (!rt)                      printf("%s", data);
    if (ss && n < ss)        n += printf("%*s", MAX(0, ss - n), " ");
    if (rt)                       printf("%s", data);
    if (e)                   n += printf_eol();

    return n;
  }
#undef PRINT2
#undef PRINT1
  return -1;
}

int printf_data(const void* p, const size_t size, const addrz_t addr, const modez_t mode) {
#define MAX_SIZE (16)
  addrz_t x = addr;
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

      printf_eol();
    } else if (USE_STR == mode) {
      if (0 == *pp) return i;
      printf_nice(*pp, USE_CHARCTRL);
      ++pp;
      ++i;
    } else if (USE_HEX == mode) {
      printf_nice(*pp, USE_LHEX8NS);
      ++pp;
      ++i;
    } else {
      return -1;
    }
  }
#undef MAX_SIZE
  return i;
}

int printf_mask(const pconvert_t p, const maskz_t mask, const modez_t mode) {
  int n = 0;
  maskz_t v = mask;
  for (pconvert_t x = p; 0 != x->text; ++x) {
    if (x->type & v) {
      n += printf_text(x->text, (mode & ~USE_EOL) | USE_SPACE);
      v &= ~x->type;
    }
  }

  if (v) {
    n += printf_nice(v, USE_UNKNOWN | USE_SPACE | (mode & ~USE_EOL));
  }

  if (mode & USE_EOL) {
    n += printf_eol();
  }

  return n;
}

int printf_maskmute(const pconvert_t p, const maskz_t mask, const modez_t mode) {
  int n = 0;
  for (pconvert_t x = p; 0 != x->text; ++x) {
    if (x->type & mask) {
      n += printf_text(x->text, (mode & ~USE_EOL) | USE_SPACE);
    }
  }

  if (mode & USE_EOL) {
    n += printf_eol();
  }

  return n;
}

int printf_masknone(const pconvert_t p, const maskz_t mask, const modez_t mode) {
  if (0 == mask) {
    return printf_text("NONE", USE_SPACE | mode);
  }

  return printf_mask(p, mask, USE_SPACE | mode);
}

int printf_pick(const pconvert_t p, const pick_t x, const modez_t mode) {
  return printf_text(strpick(p, x), mode);
}

int printf_pack(const size_t size) {
  return printf_text(" ", SET_PAD(size));
}

