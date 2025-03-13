#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <inttypes.h>

#include "printf.h"

#define MAX_BUFFER_SIZE (1024)

#define PRINT1(x)    snprintf(o + n, size - n, x)
#define PRINT2(x,y)  snprintf(o + n, size - n, x, y)

static char errname[256] = {0};

void set_errname(const char* name) {
  strncpy(errname, name, sizeof(errname));
}

int printf_work(char* o, const size_t size, const char* p, const imode_t mode) {
  int n = 0;
  if (o && p) {
    int b = GET_BRACKET(mode);
    int p0 = GET_POS0(mode);
    int p1 = GET_POS1(mode);

    switch (p0) {
    case USE_AT:             n += PRINT1("@");          break;
    case USE_ATAT:           n += PRINT1("@@");         break;
    case USE_SPACE:          n += PRINT1(" ");          break;
    case USE_TAB:            n += PRINT1("  ");         break;
    default:                 break;
    }

    switch (b) {
    case USE_SBLT:           n += PRINT2("[%s",  p);    break;
    case USE_SBRT:           n += PRINT2("%s]",  p);    break;
    case USE_CB:             n += PRINT2("{%s}", p);    break;
    case USE_RB:             n += PRINT2("(%s)", p);    break;
    case USE_SB:             n += PRINT2("[%s]", p);    break;
    case USE_TB:             n += PRINT2("<%s>", p);    break;
    case USE_DRTB:           n += PRINT2(">>%s<<", p);  break;
    case USE_SQ:             n += PRINT2("'%s'", p);    break;
    case USE_DQ:             n += PRINT2("\"%s\"", p);  break;
    case USE_DQEQ:           n += PRINT2("\"=%s\"", p); break;
    case USE_PLUS:           n += PRINT2(" +%s", p);    break;
    case USE_DASH:           n += PRINT2(" -%s", p);    break;
    default:                 n += PRINT2("%s", p);      break;
    }

    switch (p1) {
    case USE_COLON:          n += PRINT1(":");          break;
    case USE_BYTES:          n += PRINT1(" (bytes)");   break;
    default:                 break;
    }
  }

  return n;
}

int printf_neat(char* o, const size_t size, const uint64_t v, const imode_t mode) {
  int n = 0;
  if (o) {
    switch (GET_POS0(mode)) {
    case USE_TAB:                  n += PRINT1(" ");     break;
    default:                       break;
    }

    const imode_t xmode = mode & ~(USE_FLAGMASK | USE_POS0MASK | USE_POS1MASK | USE_BRACKETMASK);
    const int usespace = (USE_CHARCTRL != xmode && USE_LHEX8NS != xmode && USE_CHAR != xmode) || mode & USE_SPACE;

    switch (GET_BRACKET(mode)) {
    case USE_CB:                   n += PRINT1(" {");    break;
    case USE_RB:                   n += PRINT1(" (");    break;
    case USE_SB:                   n += PRINT1(" [");    break;
    case USE_TB:                   n += PRINT1(" <");    break;
    case USE_DRTB:                 n += PRINT1( ">>");   break;
    case USE_SQ:                   n += PRINT1(" '");    break;
    case USE_DQ:                   n += PRINT1(" \"");   break;
    case USE_PLUS:                 n += PRINT1(" +");    break;
    case USE_DASH:                 n += PRINT1(" -");    break;
    default:
      if (usespace)                n += PRINT1(" ");
      break;
    }

    switch (xmode) {
    case USE_DEC:                  n += PRINT2("%" PRId64, v);                     break;
    case USE_DEC2:                 n += PRINT2("%2" PRId64, v);                    break;
    case USE_DEC2Z:                n += PRINT2("%2.2" PRId64, v);                  break;
    case USE_DEC3:                 n += PRINT2("%3" PRId64, v);                    break;
    case USE_DEC3Z:                n += PRINT2("%3.3" PRId64, v);                  break;
    case USE_DEC5:                 n += PRINT2("%5" PRId64, v);                    break;
    case USE_DEC5Z:                n += PRINT2("%5.5" PRId64, v);                  break;
    case USE_OCT:                  n += PRINT2("%" PRIo64, v);                     break;
    case USE_OCT2:                 n += PRINT2("%2" PRIo64, v);                    break;
    case USE_OCT5:                 n += PRINT2("%5" PRIo64, v);                    break;
    case USE_FHEX:                 n += PRINT2("0x%" PRIx64, v);                   break;
    case USE_LHEX:                 n += PRINT2("%" PRIx64, v);                     break;
    case USE_FHEX8:                n += PRINT2("0x%2.2" PRIx64, v);                break;
    case USE_LHEX8:                n += PRINT2("%2.2" PRIx64, v);                  break;
    case USE_LHEX8NS:              n += PRINT2("%2.2" PRIx64, v);                  break;
    case USE_FHEX16:               n += PRINT2("0x%4.4" PRIx64, v);                break;
    case USE_LHEX16:               n += PRINT2("%4.4" PRIx64, v);                  break;
    case USE_FHEX24:               n += PRINT2("0x%6.6" PRIx64, v);                break;
    case USE_LHEX24:               n += PRINT2("%6.6" PRIx64, v);                  break;
    case USE_FHEX32:               n += PRINT2("0x%8.8" PRIx64, v);                break;
    case USE_LHEX32:               n += PRINT2("%8.8" PRIx64, v);                  break;
    case USE_FHEX48:               n += PRINT2("0x%12.12" PRIx64, v);              break;
    case USE_LHEX48:               n += PRINT2("%12.12" PRIx64, v);                break;
    case USE_FHEX64:               n += PRINT2("0x%16.16" PRIx64, v);              break;
    case USE_LHEX64:               n += PRINT2("%16.16" PRIx64, v);                break;
    case USE_POWER2:               n += PRINT2("2**%-2" PRId64, v);                break;
    case USE_PERCENT:              n += PRINT2("%3.1f%%", CAST(double, v) / 10);   break;
    case USE_ERROR:                n += PRINT2("<error: %" PRIx64 ">", v);         break;
    case USE_CORRUPT:              n += PRINT2("<corrupt: %" PRIx64 ">", v);       break;
    case USE_UNKNOWN:              n += PRINT2("<unknown: %" PRIx64 ">", v);       break;
    case USE_WARNING:              n += PRINT2("<warning: %" PRIx64 ">", v);       break;

    case USE_SHEX8:
      if (v < CHAR_MAX)            n += PRINT2("+ %" PRIx64, v);
      else                         n += PRINT2("- %" PRIx64, (~v) + 1);
      break;

    case USE_SHEX16:
      if (v < SHRT_MAX)            n += PRINT2("+ %" PRIx64, v);
      else                         n += PRINT2("- %" PRIx64, (~v) + 1);
      break;

    case USE_SHEX32:
      if (v < INT_MAX)             n += PRINT2("+ %" PRIx64, v);
      else                         n += PRINT2("- %" PRIx64, (~v) + 1);
      break;

    case USE_SHEX64:
      if (v < LONG_MAX)            n += PRINT2("+ %" PRIx64, v);
      else                         n += PRINT2("- %" PRIx64, (~v) + 1);
      break;

    case USE_CHAR:
      if (isprint(v))              n += PRINT2("%c", CAST(int, v));
      else                         n += PRINT1(".");
      break;

    case USE_CHARCTRL:
      if (iscntrl(v))              n += PRINT2("^%c", CAST(int, v) + 0x40);
      else if (isprint(v))         n += PRINT2("%c", CAST(int, v));
      else                         n += PRINT1(".");
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
    case USE_CB:                   n += PRINT1("}");         break;
    case USE_RB:                   n += PRINT1(")");         break;
    case USE_SB:                   n += PRINT1("]");         break;
    case USE_TB:                   n += PRINT1(">");         break;
    case USE_DRTB:                 n += PRINT1("<<");        break;
    case USE_SQ:                   n += PRINT1("'");         break;
    case USE_DQ:                   n += PRINT1("\"");        break;
    default:                       break;
    }

    switch (GET_POS1(mode)) {
    case USE_COLON:                n += PRINT1(":");         break;
    case USE_BYTES:                n += PRINT1(" (bytes)");  break;
    default:                       break;
    }
  }

  return n;
}

int printf_eol() {
  return printf("\n");
}

int printf_nice(const uint64_t v, const imode_t mode) {
  MALLOCA(char, data, 1024);
  int n = printf_neat(data, sizeof(data), v, mode);

  printf("%s", data);

  if (mode & USE_EOL) {
    n += printf_eol();
  }

  return n;
}

int printf_text(const char* p, const imode_t mode) {
  MALLOCA(char, data, MAX_BUFFER_SIZE);
  int n = printf_work(data, sizeof(data), p, mode);
  if (p) {
    int e = mode & USE_EOL;
    int ss = CAST(int, GET_PAD(mode));
    int rt = USE_RT == GET_FORMAT(mode);

    if (!rt)                      printf("%s", data);
    if (ss && n < ss)        n += printf_pack(MAX(0, ss - n));
    if (rt)                       printf("%s", data);
    if (e)                   n += printf_eol();

    return n;
  }

  return -1;
}

int printf_book(const char* p[], const imode_t mode) {
  if (p) {
    int n = 0;

    for (int i = 0; 0 != p[i]; ++i) {
      n += printf_text(p[i], mode);
    }

    return n;
  }

  return -1;
}

int printf_data(const void* p, const size_t size, const addrz_t addr, const imode_t mode) {
  const size_t MAX_SIZE = 16;

  const imode_t xmode = mode & ~(USE_POS0MASK | USE_FLAGMASK);
  const int usespace = mode & USE_SPACE;

  addrz_t x = addr;
  int n = 0;
  size_t i = 0;
  const unsigned char *pp = CAST(unsigned char*, p);
  for (i = 0; i < size; ) {
    if (USE_HEXDUMP == xmode) {
      n += printf_nice(x, USE_FHEX32);

      size_t siz = (size - i) > MAX_SIZE ? MAX_SIZE : (size - i);
      for (size_t j = 0; j < MAX_SIZE; j++) {
        if (j < siz) n += printf_nice(pp[j], USE_LHEX8);
        else n += printf_pack(3);
      }

      n += printf_pack(3);

      for (size_t k = 0; k < siz; ++k) {
        n += printf_nice(pp[k], USE_CHAR);
      }

      n += printf_eol();

      pp += siz;
      i += siz;
      x += siz;
    } else if (USE_STRDUMP == xmode) {
      while (!isprint(*pp) && i < size) {
        ++pp;
        ++i;
      }

      n += printf_nice(i, USE_TAB | USE_SB | USE_DEC5);
      n += printf_pack(2);

      while (0 != *pp && i < size) {
        n += printf_nice(*pp, USE_CHARCTRL);
        ++pp;
        ++i;
      }

      n += printf_eol();
      ++pp;
      ++i;
    } else if (USE_STR == xmode) {
      if (0 == *pp) return n;
      n += printf_nice(*pp, USE_CHARCTRL);
      ++pp;
      ++i;
    } else if (USE_HEX == xmode) {
      n += printf_nice(*pp, usespace ? USE_LHEX8 : USE_LHEX8NS);
      ++pp;
      ++i;
    } else {
      return -1;
    }
  }

  if (mode & USE_EOL) {
    n += printf_eol();
  }

  return n;
}

int printf_mask(const pconvert_t p, const maskz_t mask, const imode_t mode) {
  MALLOCA(char, data, MAX_BUFFER_SIZE);

  int n = 0;
  maskz_t v = mask;
  imode_t s = mode & USE_NOSPACE ? 0 : USE_SPACE;
  for (pconvert_t x = p; 0 != x->text; ++x) {
    if (x->type & v) {
      n += printf_work(data + n, sizeof(data) - n, x->text, (mode & ~USE_EOL) | s);
      v &= ~x->type;
    }
  }

  if (v) {
    n += printf_neat(data + n, sizeof(data) - n, v, USE_UNKNOWN | USE_SPACE | (mode & ~USE_EOL));
  }

  return printf_text(data, mode);
}

int printf_maskmute(const pconvert_t p, const maskz_t mask, const imode_t mode) {
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

int printf_masknone(const pconvert_t p, const maskz_t mask, const imode_t mode) {
  if (0 == mask) {
    return printf_text("NONE", USE_SPACE | mode);
  }

  return printf_mask(p, mask, USE_SPACE | mode);
}

int printf_pick(const pconvert_t p, const pick_t x, const imode_t mode) {
  return printf_text(strpick(p, x), mode);
}

int printf_pack(const int size) {
  return size <= 0 ? 0 : printf("%*s", size, " ");
}

int printf_d(const char* format, ...) {
  MALLOCA(char, data, MAX_BUFFER_SIZE);

  va_list pVAList;
  va_start(pVAList, format);
  vsnprintf(data, sizeof(data), format, pVAList);
  va_end(pVAList);

  if (errname[0]) {
    return fprintf(stdout, "%s: DEBUG: %s\n\n", errname, data);
  }

  return fprintf(stdout, "DEBUG: %s\n\n", data);
}

int printf_e(const char* format, ...) {
  MALLOCA(char, data, MAX_BUFFER_SIZE);

  va_list pVAList;
  va_start(pVAList, format);
  vsnprintf(data, sizeof(data), format, pVAList);
  va_end(pVAList);

  if (errname[0]) {
    return fprintf(stderr, "%s: ERROR: %s\n\n", errname, data);
  }

  return fprintf(stderr, "ERROR: %s\n\n", data);
}

int printf_i(const char* format, ...) {
  MALLOCA(char, data, MAX_BUFFER_SIZE);

  va_list pVAList;
  va_start(pVAList, format);
  vsnprintf(data, sizeof(data), format, pVAList);
  va_end(pVAList);

  if (errname[0]) {
    return fprintf(stdout, "%s: INFO: %s\n\n", errname, data);
  }

  return fprintf(stdout, "INFO: %s\n\n", data);
}

int printf_w(const char* format, ...) {
  MALLOCA(char, data, MAX_BUFFER_SIZE);

  va_list pVAList;
  va_start(pVAList, format);
  vsnprintf(data, sizeof(data), format, pVAList);
  va_end(pVAList);

  if (errname[0]) {
    return fprintf(stdout, "%s: WARNING: %s\n\n", errname, data);
  }

  return fprintf(stdout, "WARNING: %s\n\n", data);
}

void printf_x(const char* format, ...) {
  MALLOCA(char, data, MAX_BUFFER_SIZE);

  va_list pVAList;
  va_start(pVAList, format);
  vsnprintf(data, sizeof(data), format, pVAList);
  va_end(pVAList);

  if (errname[0]) {
    fprintf(stderr, "%s: FAIL: %s\n\n", errname, data);
  } else {
    fprintf(stderr, "ERROR: %s\n\n", data);
  }
  abort();
}

