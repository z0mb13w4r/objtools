#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <inttypes.h>

#include "printf.h"

#include "static/color.ci"

#define MAX_BUFFER_SIZE  (1024)
#define MAX_PADDING_SIZE (256)

#define PRINT1(x)    snprintf(o + n, size - n, x)
#define PRINT2(x,y)  snprintf(o + n, size - n, x, y)

static char errname[256] = {0};

static int printf_post(const char* o, const imode_t mode) {
  int n = 0;

  if (o) {
    n += printf_color(mode);
    fprintf(stdout, "%s", o);
    n += printf_color(GET_COLOR(mode) ? USE_RESET : 0);
  }

  if (mode & USE_EOL) {
    n += printf_eol();
  }

  return n;
}

void set_errname(const char* name) {
  strncpy(errname, name, sizeof(errname));
}

int printf_spos(char* o, const size_t size, const imode_t mode, const bool_t usespace) {
  int n = 0;
  if (o) {
    switch (GET_POS0(mode)) {
    case USE_AT:                   n += PRINT1("@");     break;
    case USE_ATAT:                 n += PRINT1("@@");    break;
    case USE_DOT:                  n += PRINT1(".");     break;
    case USE_TAB:                  n += PRINT1(" ");     break;
    case USE_TAB2:                 n += PRINT1("   ");   break;
    default:                       break;
    }

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
  }

  return n;
}

int printf_epos(char* o, const size_t size, const imode_t mode) {
  int n = 0;
  if (o) {
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

int printf_color(const imode_t mode) {
  int n = 0;
  const char* p = NULL;
  if (GET_COLOR(mode) && (p = strpicknull(zCOLORS, GET_COLOR(mode)))) {
    n += printf("%s", p);
  }

  return n;
}

int printf_work(char* o, const size_t size, const char* p, const imode_t mode) {
  int n = 0;
  if (o && p) {
    const imode_t b = GET_BRACKET(mode);
    const imode_t p0 = GET_POS0(mode);
    const imode_t p1 = GET_POS1(mode);

    switch (p0) {
    case USE_AT:             n += PRINT1("@");          break;
    case USE_ATAT:           n += PRINT1("@@");         break;
    case USE_SPACE:          n += PRINT1(" ");          break;
    case USE_TAB:            n += PRINT1("  ");         break;
    case USE_TAB2:           n += PRINT1("    ");       break;
    case USE_DOT:            n += PRINT1(".");          break;
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
    const imode_t pos0 = GET_POS0(mode);
    const imode_t xmode = mode & ~(USE_FLAGMASK | USE_POS0MASK | USE_POS1MASK | USE_BRACKETMASK | USE_COLORMASK);
    const bool_t  usespace = (!pos0 && USE_CHARCTRL != xmode && USE_LHEX8NS != xmode && USE_CHAR != xmode)
                        || USE_SPACE == pos0 || USE_TAB == pos0;

    n += printf_spos(o + n, size - n, mode, usespace);

    switch (xmode) {
    case USE_DEC:                  n += PRINT2("%" PRId64, v);                     break;
    case USE_DEC2:                 n += PRINT2("%2" PRId64, v);                    break;
    case USE_DEC2Z:                n += PRINT2("%2.2" PRId64, v);                  break;
    case USE_DEC3:                 n += PRINT2("%3" PRId64, v);                    break;
    case USE_DEC3Z:                n += PRINT2("%3.3" PRId64, v);                  break;
    case USE_DEC5:                 n += PRINT2("%5" PRId64, v);                    break;
    case USE_DEC5Z:                n += PRINT2("%5.5" PRId64, v);                  break;
    case USE_HEX2:                 n += PRINT2("%2" PRIx64, v);                    break;
    case USE_HEX4:                 n += PRINT2("%4" PRIx64, v);                    break;
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
      if (tmp == NULL)             n += printf_nice(atime, USE_CORRUPT);
      else                         n += snprintf(o + n, size - n, "%04u-%02u-%02uT%02u:%02u:%02u",
                                                tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
                                                tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
      }
      break;

    default:
      break;
    }

    n += printf_epos(o + n, size - n, mode);
  }

  return n;
}

int printf_eol() {
  return fprintf(stdout, "\n");
}

int printf_pack(const int size) {
  return size <= 0 ? 0 : fprintf(stdout, "%*s", size, " ");
}

int printf_nice(const uint64_t v, const imode_t mode) {
  MALLOCA(char, data, 1024);

  int n = 0;
  n += printf_neat(data, sizeof(data), v, mode);
  n += printf_post(data, mode);

  return n;
}

int printf_text(const char* p, const imode_t mode) {
  MALLOCA(char, o1, MAX_BUFFER_SIZE);
  MALLOCA(char, o2, MAX_BUFFER_SIZE + MAX_PADDING_SIZE);

  int n = 0;

  if (p) {
    n += printf_work(o1, sizeof(o1), p, mode);

    int sz = MIN(MAX(0, CAST(int, GET_PAD(mode)) - n), MAX_PADDING_SIZE);
    if (sz) {
      if (USE_RT == GET_FORMAT(mode)) {
        snprintf(o2, sizeof(o2), "%*s%s", sz, " ", o1);
      } else {
        snprintf(o2, sizeof(o2), "%s%*s", o1, sz, " ");
      }

      n += printf_post(o2, mode) + sz;
    } else {
      n += printf_post(o1, mode);
    }
  }

  return n;
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

int printf_sore(const void* p, const size_t size, const imode_t mode) {
  const imode_t xmode = mode & ~(USE_POS0MASK | USE_FLAGMASK | USE_COLORMASK);
  const imode_t zmode = mode &   USE_POS0MASK;

  int n = 0;
  if (USE_HASHALL == xmode) {
    n += printf_text("HASHES", USE_LT | USE_COLON | USE_EOL);
    n += printf_sore(p, size, USE_MD5 | USE_TAB | zmode);
    n += printf_sore(p, size, USE_SHA1 | USE_TAB | zmode);
    n += printf_sore(p, size, USE_SHA256 | USE_TAB | zmode);
    n += printf_sore(p, size, USE_SHA512 | USE_TAB | zmode);
    n += printf_eol();
  }

  MALLOCA(char, o, 1024);

  puchar_t p0 = CAST(puchar_t, p);
  if (USE_STR == xmode) {
    n += printf_spos(o, sizeof(o), mode, USE_SPACE == GET_POS0(mode) || USE_TAB == GET_POS0(mode));

    for (size_t i = 0; i < size; ++i, ++p0) {
      if (0 == *p0) break;
      n += printf_neat(o + n, sizeof(o) - n, *p0, USE_CHARCTRL);
    }

    n += printf_epos(o, sizeof(o), mode);
    n += printf_post(o, mode);
  } else if (USE_HEX == xmode) {
    n += printf_spos(o, sizeof(o), mode, USE_SPACE == GET_POS0(mode) || USE_TAB == GET_POS0(mode));

    for (size_t i = 0; i < size; ++i, ++p0) {
      n += printf_neat(o + n, sizeof(o) - n, *p0, USE_SPACE == GET_POS0(mode) ? USE_LHEX8 : USE_LHEX8NS);
    }

    n += printf_epos(o, sizeof(o), mode);
    n += printf_post(o, mode);
  } else if (USE_MD5 == xmode) {
    uchar_t md[MD5_DIGEST_LENGTH];
    if (!md5(p0, size, md)) {
      printf_text("MD5", USE_LT | USE_COLON | zmode | SET_PAD(10));
      printf_sore(md, MD5_DIGEST_LENGTH, USE_HEX | USE_EOL);
    }
  } else if (USE_SHA1 == xmode) {
    uchar_t md[SHA_DIGEST_LENGTH];
    if (!sha1(p0, size, md)) {
      printf_text("SHA1", USE_LT | USE_COLON | zmode | SET_PAD(10));
      printf_sore(md, SHA_DIGEST_LENGTH, USE_HEX | USE_EOL);
    }
  } else if (USE_SHA256 == xmode) {
    uchar_t md[SHA256_DIGEST_LENGTH];
    if (!sha256(p0, size, md)) {
      printf_text("SHA256", USE_LT | USE_COLON | zmode | SET_PAD(10));
      printf_sore(md, SHA256_DIGEST_LENGTH, USE_HEX | USE_EOL);
    }
  } else if (USE_SHA512 == xmode) {
    uchar_t md[SHA512_DIGEST_LENGTH];
    if (!sha512(p0, size, md)) {
      printf_text("SHA512", USE_LT | USE_COLON | zmode | SET_PAD(10));
      printf_sore(md, SHA512_DIGEST_LENGTH, USE_HEX | USE_EOL);
    }
  } else if (USE_CRC32 == xmode) {

  } else if (USE_BASE64 == xmode) {

  } else if (USE_GUID == xmode) {
    n += printf_spos(o, sizeof(o), mode, USE_SPACE == GET_POS0(mode) || USE_TAB == GET_POS0(mode));

    for (size_t i = 0; i < 4; ++i, ++p0) n += printf_neat(o + n, sizeof(o) - n, *p0, USE_LHEX8NS);
    n += printf_neat(o + n, sizeof(o) - n, '-', USE_CHAR);
    for (size_t i = 0; i < 2; ++i, ++p0) n += printf_neat(o + n, sizeof(o) - n, *p0, USE_LHEX8NS);
    n += printf_neat(o + n, sizeof(o) - n, '-', USE_CHAR);
    for (size_t i = 0; i < 2; ++i, ++p0) n += printf_neat(o + n, sizeof(o) - n, *p0, USE_LHEX8NS);
    n += printf_neat(o + n, sizeof(o) - n, '-', USE_CHAR);
    for (size_t i = 0; i < 8; ++i, ++p0) n += printf_neat(o + n, sizeof(o) - n, *p0, USE_LHEX8NS);

    n += printf_epos(o, sizeof(o), mode);
    n += printf_post(o, mode);
  }

  return n;
}

int printf_data(const void* p, const size_t size, const addrz_t addr, const imode_t mode) {
  const size_t MAX_SIZE = 16;

  const imode_t xmode = mode & ~(USE_POS0MASK | USE_FLAGMASK);
  const bool_t usespace = GET_POS0(mode) == USE_SPACE;

  int n = 0;
  if (USE_TAB == GET_POS0(mode)) {
    n += printf_pack(1);
  }

  addrz_t x = addr;
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

      n += printf_nice(i, USE_TAB | USE_SB | USE_LHEX16);
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
      if (0 == *pp) break;
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
  if (p) {
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

  if (mode & USE_EOL) {
    n += printf_eol();
  }

  return n;
}

int printf_maskmute(const pconvert_t p, const maskz_t mask, const imode_t mode) {
  int n = 0;
  if (p) {
    for (pconvert_t x = p; 0 != x->text; ++x) {
      if (x->type & mask) {
        n += printf_text(x->text, (mode & ~USE_EOL) | USE_SPACE);
      }
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

