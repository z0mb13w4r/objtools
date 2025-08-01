#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <inttypes.h>

#include "decode.h"
#include "printf.h"

#include "static/color.ci"

#define STDERR stdout
#define STDOUT stdout

#define MAX_BUFFER_SIZE  (1024)
#define MAX_PADDING_SIZE (256)

#define PRINT1(x)    snprintf(o + n, size - n, x)
#define PRINT2(x,y)  snprintf(o + n, size - n, x, y)

#define MODE_USESPACE(x)   (USE_SPACE == GET_POS0(x))
#define MODE_USESPACES(x)  (USE_SPACE == GET_POS0(x) || USE_TAB1 == GET_POS0(x) || USE_TAB2 == GET_POS0(x))

static char errname[256] = {0};

static imode_t make_spos(const imode_t mode) {
  switch (GET_BRACKET(mode)) {
  case USE_CB:                     return USE_CBLT;
  case USE_RB:                     return USE_RBLT;
  case USE_SB:                     return USE_SBLT;
  case USE_TB:                     return USE_TBLT;
  default:
    break;
  }

  return USE_NONE;
}

static imode_t make_epos(const imode_t mode) {
  switch (GET_BRACKET(mode)) {
  case USE_CB:                     return USE_CBRT;
  case USE_RB:                     return USE_RBRT;
  case USE_SB:                     return USE_SBRT;
  case USE_TB:                     return USE_TBRT;
  default:
    break;
  }

  return USE_NONE;
}

static int printf_post(const char* o, const imode_t mode) {
  int n = 0;

  if (o) {
    n += printf_color(mode);
    fprintf(STDOUT, "%s", o);
    n += printf_color(GET_COLOR(mode) ? USE_RESET : 0);
  }

  if (mode & USE_EOL) {
    n += printf_eol();
  }

  return n;
}

int printf_errname(const char* name) {
  strncpy(errname, name, sizeof(errname));
  return 0;
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
    case USE_COMMA:                n += PRINT1(", ");    break;
    case USE_0x:                   n += PRINT1("0x");    break;
    default:                       break;
    }

    switch (GET_BRACKET(mode)) {
    case USE_DRTB:                 n += PRINT1(">>");    break;
    case USE_SQ:                   n += PRINT1(" '");    break;
    case USE_DQ:                   n += PRINT1(" \"");   break;
    case USE_LINE:                 n += PRINT1(" (line ");     break;
    case USE_OFFSET:               n += PRINT1(" (offset ");   break;
    case USE_ADDRESS:              n += PRINT1(" (address ");  break;
    case USE_DISCRIMINATOR:        n += PRINT1(" (discriminator "); break;
    default:
      if (usespace)                n += PRINT1(" ");
      switch (GET_BRACKET(mode)) {
      case USE_CB:
      case USE_CBLT:               n += PRINT1("{");     break;
      case USE_RB:
      case USE_RBLT:               n += PRINT1("(");     break;
      case USE_SB:
      case USE_SBLT:               n += PRINT1("[");     break;
      case USE_TB:
      case USE_TBLT:               n += PRINT1("<");     break;
      case USE_PLUS:               n += PRINT1("+");     break;
      case USE_DASH:               n += PRINT1("-");     break;
      default:
        break;
      }
      break;
    }
  }

  return n;
}

int printf_epos(char* o, const size_t size, const imode_t mode) {
  int n = 0;
  if (o) {
    switch (GET_BRACKET(mode)) {
    case USE_CB:
    case USE_CBRT:                 n += PRINT1("}");         break;
    case USE_LINE:
    case USE_OFFSET:
    case USE_ADDRESS:
    case USE_DISCRIMINATOR:
    case USE_RB:
    case USE_RBRT:                 n += PRINT1(")");         break;
    case USE_SB:
    case USE_SBRT:                 n += PRINT1("]");         break;
    case USE_TB:
    case USE_TBRT:                 n += PRINT1(">");         break;
    case USE_DRTB:                 n += PRINT1("<<");        break;
    case USE_SQ:                   n += PRINT1("'");         break;
    case USE_DQ:                   n += PRINT1("\"");        break;
    default:                       break;
    }

    switch (GET_POS1(mode)) {
    case USE_COLON:                n += PRINT1(":");         break;
    case USE_SEMICOLON:            n += PRINT1(";");         break;
    case USE_BYTES:                n += PRINT1(" (bytes)");  break;
    case USE_COUNT:                n += PRINT1(" (count)");  break;
    case USE_MIN:                  n += PRINT1(" (min)");    break;
    case USE_MAX:                  n += PRINT1(" (max)");    break;
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
    case USE_TAB1:           n += PRINT1("  ");         break;
    case USE_TAB2:           n += PRINT1("    ");       break;
    case USE_DOT:            n += PRINT1(".");          break;
    case USE_COMMA:          n += PRINT1(", ");         break;
    default:                 break;
    }

    switch (b) {
    case USE_CB:             n += PRINT2("{%s}", p);    break;
    case USE_CBLT:           n += PRINT2("{%s",  p);    break;
    case USE_CBRT:           n += PRINT2("%s)",  p);    break;
    case USE_RB:             n += PRINT2("(%s)", p);    break;
    case USE_RBLT:           n += PRINT2("(%s",  p);    break;
    case USE_RBRT:           n += PRINT2("%s)",  p);    break;
    case USE_SB:             n += PRINT2("[%s]", p);    break;
    case USE_SBLT:           n += PRINT2("[%s",  p);    break;
    case USE_SBRT:           n += PRINT2("%s]",  p);    break;
    case USE_TB:             n += PRINT2("<%s>", p);    break;
    case USE_TBLT:           n += PRINT2("<%s",  p);    break;
    case USE_TBRT:           n += PRINT2("%s>",  p);    break;
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
    case USE_SEMICOLON:      n += PRINT1(";");          break;
    case USE_BYTES:          n += PRINT1(" (bytes)");   break;
    case USE_COUNT:          n += PRINT1(" (count)");   break;
    case USE_MIN:            n += PRINT1(" (min)");     break;
    case USE_MAX:            n += PRINT1(" (max)");     break;
    default:                 break;
    }
  }

  return n;
}

int printf_tidy(char* o, const size_t size, const double v, const imode_t mode) {
  int n = 0;
  if (o) {
    const imode_t mode0 = GET_POS0(mode);
    const imode_t modex = GET_STYLE(mode);
    const bool_t  usespace = (0 == (mode & USE_NOSPACE) && 0 == mode0) || MODE_USESPACES(mode0);

    n += printf_spos(o + n, size - n, mode, usespace);

    switch (modex) {
    case USE_REALp1:               n += PRINT2("%.1f", v);                         break;
    case USE_REALp2:               n += PRINT2("%.2f", v);                         break;
    case USE_REALp4:               n += PRINT2("%.4f", v);                         break;
    case USE_REALp6:               n += PRINT2("%.6f", v);                         break;

    default:
      break;
    }

    n += printf_epos(o + n, size - n, mode);
  }

  return n;
}

int printf_neat(char* o, const size_t size, const uint64_t v, const imode_t mode) {
  int n = 0;
  if (o) {
    const imode_t mode0 = GET_POS0(mode);
    const imode_t modex = GET_STYLE(mode);
    const bool_t  usespace = (0 == (mode & USE_NOSPACE) && 0 == mode0 && USE_CHARCTRL != modex && USE_CHAR != modex)
                        || MODE_USESPACES(mode0);

    n += printf_spos(o + n, size - n, mode, usespace);

    switch (modex) {
    case USE_DEC:                  n += PRINT2("%" PRId64, v);                     break;
    case USE_DEC2:                 n += PRINT2("%2" PRId64, v);                    break;
    case USE_DEC2Z:                n += PRINT2("%2.2" PRId64, v);                  break;
    case USE_DEC3:                 n += PRINT2("%3" PRId64, v);                    break;
    case USE_DEC3Z:                n += PRINT2("%3.3" PRId64, v);                  break;
    case USE_DEC4:                 n += PRINT2("%4" PRId64, v);                    break;
    case USE_DEC4Z:                n += PRINT2("%4.4" PRId64, v);                  break;
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
    case USE_BOOL:                 n += PRINT1(v ? "true" : "false");              break;

    case USE_SDEC8:
      if (v < CHAR_MAX)            n += PRINT2("%" PRId64, v);
      else                         n += PRINT2("%" PRId64, -(UCHAR_MAX - v) - 1);
      break;

    case USE_SDEC16:
      if (v < SHRT_MAX)            n += PRINT2("%" PRId64, v);
      else                         n += PRINT2("%" PRId64, -(USHRT_MAX - v) - 1);
      break;

    case USE_SDEC32:
      if (v < INT_MAX)             n += PRINT2("%" PRId64, v);
      else                         n += PRINT2("%" PRId64, -(UINT_MAX - v) - 1);
      break;

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

    case USE_YESNO:
      if (v)                       n += PRINT2("yes(%" PRId64 ")", v);
      else                         n += PRINT2("no(%" PRId64 ")", v);
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
  return fprintf(STDOUT, "\n");
}

int printf_pack(const int size) {
  return size <= 0 ? 0 : fprintf(STDOUT, "%*s", size, " ");
}

int printf_open(const imode_t mode) {
  MALLOCA(char, o, MAX_BUFFER_SIZE);

  int n = 0;
  n += printf_spos(o, sizeof(o), mode, MODE_USESPACES(mode));
  n += printf_post(o, mode);

  return n;
}

int printf_stop(const imode_t mode) {
  MALLOCA(char, o, MAX_BUFFER_SIZE);

  int n = 0;
  n += printf_epos(o, sizeof(o), mode);
  n += printf_post(o, mode);

  return n;
}

int printf_nice(const uint64_t v, const imode_t mode) {
  MALLOCA(char, o, MAX_BUFFER_SIZE);

  int n = 0;
  n += printf_neat(o, sizeof(o), v, mode);
  n += printf_post(o, mode);

  return n;
}

int printf_real(const double v, const imode_t mode) {
  MALLOCA(char, o, MAX_BUFFER_SIZE);

  int n = 0;
  n += printf_tidy(o, sizeof(o), v, mode);
  n += printf_post(o, mode);

  return n;
}

int printf_join(const char* p, const uint64_t v, const imode_t mode) {
  MALLOCA(char, o, MAX_BUFFER_SIZE);

  int n = 0;
  if (p) {
    const imode_t modex = mode & ~(USE_BRACKETMASK | USE_POS0MASK | USE_POS1MASK);
    const imode_t mode0 = GET_POS0(mode) | make_spos(mode);
    const imode_t mode1 = GET_POS1(mode) | make_epos(mode);

    n += printf_work(o, sizeof(o), p, modex | mode0);
    n += printf_neat(o + n, sizeof(o) - n, v, modex | mode1 | USE_NOSPACE);
    n += printf_post(o, modex);
  }

  return n;
}

int printf_yoke(const char* p, const char* q, const imode_t mode) {
  MALLOCA(char, o, MAX_BUFFER_SIZE);

  int n = 0;
  if (p) {
    const imode_t modex = mode & ~(USE_BRACKETMASK | USE_POS0MASK | USE_POS1MASK);
    const imode_t mode0 = GET_POS0(mode) | make_spos(mode);
    const imode_t mode1 = GET_POS1(mode) | make_epos(mode);

    n += printf_work(o, sizeof(o), p, modex | mode0);
    n += printf_work(o + n, sizeof(o) - n, q, modex | mode1 | USE_NOSPACE);
    n += printf_post(o, modex);
  }

  return n;
}

int printf_tack(const uint64_t v0, const imode_t m0, const char* v1, const uint64_t v2, const imode_t m2, const imode_t mode) {
  MALLOCA(char, o, MAX_BUFFER_SIZE);

  int n = 0;
  if (v1) {
    const imode_t mode0 = GET_POS0(mode) | make_spos(mode);
    const imode_t mode1 = GET_POS1(mode) | make_epos(mode);

    n += printf_nice(v0, m0 | mode0);
    n += printf_text(v1, USE_LT | USE_SPACE);
    n += printf_nice(v2, m2 | mode1);
  }

  return n;
}

int printf_text(const char* p, const imode_t mode) {
  MALLOCA(char, o1, MAX_BUFFER_SIZE);
  MALLOCA(char, o2, MAX_BUFFER_SIZE + MAX_PADDING_SIZE);

  int n = 0;

  n += printf_work(o1, sizeof(o1), p ? p : "", mode);

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

  return ECODE_HANDLE;
}

int printf_hurt(const unknown_t p, const size_t size, const imode_t mode) {
  int n = 0;
  if (0 != size) {
    n += printf_pack(mode & USE_TAB2 ? 4 : 0);
    n += printf_pack(mode & USE_TAB1 ? 2 : 0);
    n += printf_pack(mode & USE_SPACE ? 1 : 0);
    n += printf_sore(p, size, mode & ~(USE_SPACE | USE_TAB1 | USE_TAB2));
  }

  return n;
}

int printf_sore(const unknown_t p, const size_t size, const imode_t mode) {
  const int MAXSIZE = 10;
  const imode_t mode0 = GET_POS0(mode);
  const imode_t modex = GET_STYLE(mode);
  const imode_t ymode = mode &  (USE_POS0MASK | USE_POS1MASK | USE_FLAGMASK | USE_COLORMASK);
  int n = 0;

  if (USE_HASHALL == modex) {
    n += printf_text("HASHES", USE_LT | USE_COLON | USE_EOL);
    n += printf_sore(p, size, USE_MD5 | USE_TAB | mode0);
    n += printf_sore(p, size, USE_SHA1 | USE_TAB | mode0);
    n += printf_sore(p, size, USE_SHA256 | USE_TAB | mode0);
    n += printf_sore(p, size, USE_SHA512 | USE_TAB | mode0);
    n += printf_eol();
  } else if (USE_CRCALL == modex) {
    n += printf_text("CRCS", USE_LT | USE_COLON | USE_EOL);
    n += printf_sore(p, size, USE_CRC8 | USE_TAB | mode0);
    n += printf_sore(p, size, USE_CRC16 | USE_TAB | mode0);
    n += printf_sore(p, size, USE_CRC32 | USE_TAB | mode0);
    n += printf_eol();
  }

  MALLOCA(char, o, 1024);

  puchar_t p0 = CAST(puchar_t, p);
  if (USE_STR == modex || USE_STRSIZE == modex) {
    n += printf_spos(o, sizeof(o), mode, MODE_USESPACES(mode));

    for (size_t i = 0; i < size; ++i, ++p0) {
      if (USE_STR == modex && 0 == *p0) break;
      n += printf_neat(o + n, sizeof(o) - n, *p0, USE_CHARCTRL);
    }

    n += printf_epos(o, sizeof(o), mode);
    n += printf_post(o, mode);
  } else if (USE_STR16 == modex || USE_STR16SIZE == modex) {
    n += printf_spos(o, sizeof(o), mode, MODE_USESPACES(mode));

    pushort_t p1 = CAST(pushort_t, p);
    for (size_t i = 0; i < size; i += 2, ++p1) {
      if (USE_STR16 == modex && 0 == *p1) break;
      n += printf_neat(o + n, sizeof(o) - n, *p1, USE_CHARCTRL);
    }

    n += printf_epos(o, sizeof(o), mode);
    n += printf_post(o, mode);
  } else if (USE_HEX == modex) {
    if (0 != size) {
      n += printf_spos(o, sizeof(o), mode, MODE_USESPACES(mode));

      for (size_t i = 0; i < size; ++i, ++p0) {
        n += printf_neat(o + n, sizeof(o) - n, *p0, MODE_USESPACE(mode) ? USE_LHEX8 : USE_LHEX8 | USE_NOSPACE);
      }

      n += printf_epos(o + n, sizeof(o) - n, mode);
      n += printf_post(o, mode);
    }
  } else if (USE_MD5 == modex) {
    uchar_t md[MD5_DIGEST_LENGTH];
    if (!md5(p0, size, md)) {
      if (0 == (mode & USE_NOTEXT)) {
        n += printf_text("MD5", USE_LT | USE_COLON | mode0 | SET_PAD(MAXSIZE));
        n += printf_sore(md, MD5_DIGEST_LENGTH, USE_HEX | USE_EOL);
      } else {
        n += printf_hurt(md, MD5_DIGEST_LENGTH, USE_HEX | ymode);
      }
    } else if (0 != (mode & USE_NOTEXT)) {
      n += printf_hurt(md, MD5_DIGEST_LENGTH, USE_HEX | ymode);
    }
  } else if (USE_SHA1 == modex) {
    uchar_t md[SHA_DIGEST_LENGTH];
    if (!sha1(p0, size, md)) {
      if (0 == (mode & USE_NOTEXT)) {
        n += printf_text("SHA1", USE_LT | USE_COLON | mode0 | SET_PAD(MAXSIZE));
        n += printf_sore(md, SHA_DIGEST_LENGTH, USE_HEX | USE_EOL);
      } else {
        n += printf_hurt(md, SHA_DIGEST_LENGTH, USE_HEX | ymode);
      }
    } else if (0 != (mode & USE_NOTEXT)) {
      n += printf_hurt(md, SHA_DIGEST_LENGTH, USE_HEX | ymode);
    }
  } else if (USE_SHA256 == modex) {
    uchar_t md[SHA256_DIGEST_LENGTH];
    if (!sha256(p0, size, md)) {
      if (0 == (mode & USE_NOTEXT)) {
        n += printf_text("SHA256", USE_LT | USE_COLON | mode0 | SET_PAD(MAXSIZE));
        n += printf_sore(md, SHA256_DIGEST_LENGTH, USE_HEX | USE_EOL);
      } else {
        n += printf_hurt(md, SHA256_DIGEST_LENGTH, USE_HEX | ymode);
      }
    } else if (0 != (mode & USE_NOTEXT)) {
      n += printf_hurt(md, SHA256_DIGEST_LENGTH, USE_HEX | ymode);
    }
  } else if (USE_SHA512 == modex) {
    uchar_t md[SHA512_DIGEST_LENGTH];
    if (!sha512(p0, size, md)) {
      if (0 == (mode & USE_NOTEXT)) {
        n += printf_text("SHA512", USE_LT | USE_COLON | mode0 | SET_PAD(MAXSIZE));
        n += printf_sore(md, SHA512_DIGEST_LENGTH, USE_HEX | USE_EOL);
      } else {
        n += printf_hurt(md, SHA512_DIGEST_LENGTH, USE_HEX | ymode);
      }
    } else if (0 != (mode & USE_NOTEXT)) {
      n += printf_hurt(md, SHA512_DIGEST_LENGTH, USE_HEX | ymode);
    }
  } else if (USE_CRC8 == modex) {
    if (0 == (mode & USE_NOTEXT)) {
      n += printf_text("CRC8", USE_LT | USE_COLON | mode0 | SET_PAD(MAXSIZE));
      n += printf_nice(crc8_calculate(CRC_DEF8, p0, size), USE_FHEX8 | USE_EOL);
    } else {
      n += printf_nice(crc8_calculate(CRC_DEF8, p0, size), USE_FHEX8 | ymode);
    }
  } else if (USE_CRC16 == modex) {
    if (0 == (mode & USE_NOTEXT)) {
      n += printf_text("CRC16", USE_LT | USE_COLON | mode0 | SET_PAD(MAXSIZE));
      n += printf_nice(crc16_calculate(CRC_DEF16, p0, size), USE_FHEX16 | USE_EOL);
    } else {
      n += printf_nice(crc16_calculate(CRC_DEF16, p0, size), USE_FHEX16 | ymode);
    }
  } else if (USE_CRC32 == modex) {
    if (0 == (mode & USE_NOTEXT)) {
      n += printf_text("CRC32", USE_LT | USE_COLON | mode0 | SET_PAD(MAXSIZE));
      n += printf_nice(crc32_calculate(CRC_DEF32, p0, size), USE_FHEX32 | USE_EOL);
    } else {
      n += printf_nice(crc32_calculate(CRC_DEF32, p0, size), USE_FHEX32 | ymode);
    }
  } else if (USE_ENTROPY == modex) {
    double entropy, min, max;
    if (!entropy_calculate(p0, size, &entropy, &min, &max)) {
      if (0 == (mode & USE_NOTEXT)) {
        n += printf_text("ENTROPY", USE_LT | USE_COLON | mode0 | SET_PAD(MAXSIZE));
        n += printf_real(entropy, USE_REALp6 | USE_NOSPACE);
        n += printf_real(min, USE_REALp6 | USE_MIN);
        n += printf_real(max, USE_REALp6 | USE_MAX);
        n += printf_eol();
      } else {
        n += printf_real(entropy, USE_REALp6 | ymode);
      }
    }
  } else if (USE_ROT5 == modex) {
    if (ECODE_ISOK(rot5(p0, size))) {
      if (0 == (mode & USE_NOTEXT)) {
        n += printf_text("ROT5", USE_LT | USE_COLON | mode0 | SET_PAD(MAXSIZE));
        n += printf_sore(p0, size, USE_HEX | USE_EOL);
      } else {
        n += printf_sore(p0, size, USE_HEX | ymode);
      }
    }
  } else if (USE_ROT13 == modex) {
    if (ECODE_ISOK(rot13(p0, size))) {
      if (0 == (mode & USE_NOTEXT)) {
        n += printf_text("ROT13", USE_LT | USE_COLON | mode0 | SET_PAD(MAXSIZE));
        n += printf_sore(p0, size, USE_HEX | USE_EOL);
      } else {
        n += printf_sore(p0, size, USE_HEX | ymode);
      }
    }
  } else if (USE_ROT18 == modex) {
    if (ECODE_ISOK(rot18(p0, size))) {
      if (0 == (mode & USE_NOTEXT)) {
        n += printf_text("ROT18", USE_LT | USE_COLON | mode0 | SET_PAD(MAXSIZE));
        n += printf_sore(p0, size, USE_HEX | USE_EOL);
      } else {
        n += printf_sore(p0, size, USE_HEX | ymode);
      }
    }
  } else if (USE_BASE64 == modex) {

  } else if (USE_GUID == modex) {
    n += printf_spos(o, sizeof(o), mode, MODE_USESPACES(mode));

    for (size_t i = 0; i < 4; ++i, ++p0) n += printf_neat(o + n, sizeof(o) - n, *p0, USE_LHEX8 | USE_NOSPACE);
    n += printf_neat(o + n, sizeof(o) - n, '-', USE_CHAR);
    for (size_t i = 0; i < 2; ++i, ++p0) n += printf_neat(o + n, sizeof(o) - n, *p0, USE_LHEX8 | USE_NOSPACE);
    n += printf_neat(o + n, sizeof(o) - n, '-', USE_CHAR);
    for (size_t i = 0; i < 2; ++i, ++p0) n += printf_neat(o + n, sizeof(o) - n, *p0, USE_LHEX8 | USE_NOSPACE);
    n += printf_neat(o + n, sizeof(o) - n, '-', USE_CHAR);
    for (size_t i = 0; i < 8; ++i, ++p0) n += printf_neat(o + n, sizeof(o) - n, *p0, USE_LHEX8 | USE_NOSPACE);

    n += printf_epos(o, sizeof(o), mode);
    n += printf_post(o, mode);
  }

  return n;
}

int printf_data(const unknown_t p, const size_t size, const addrz_t addr, const imode_t mode) {
  const size_t MAX_SIZE = 16;

  const imode_t xmode = mode & ~(USE_POS0MASK | USE_FLAGMASK);

  int n = 0;
  if (USE_CODEDUMP == xmode) {
    n += printf_text("unsigned char shell_code[] = {", USE_LT | USE_EOL);
  } else if (USE_TAB == GET_POS0(mode)) {
    n += printf_pack(1);
  }

  addrz_t x = addr;
  size_t i = 0;
  puchar_t pp = CAST(puchar_t, p);
  pushort_t pu = CAST(pushort_t, p);
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
    } else if (USE_STRDUMP8 == xmode) {
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
    } else if (USE_STRDUMP16 == xmode) {
      uchar_t pc = *pu & 0xff;
      while (!isprint(pc) && i < size) {
        pc = *(++pu) & 0xff;
        i += 2;
      }

      n += printf_nice(i, USE_TAB | USE_SB | USE_LHEX16);
      n += printf_pack(2);

      while (0 != pc && i < size) {
        n += printf_nice(pc, USE_CHARCTRL);
        pc = *(++pu) & 0xff;
        i += 2;
      }

      n += printf_eol();
      ++pu;
      i += 2;
    } else if (USE_CODEDUMP == xmode) {
      size_t siz = MIN(size - i, MAX_SIZE);
      for (size_t j = 0; j < MAX_SIZE; j++) {
        n += printf_nice(pp[j], USE_FHEX8);
        if (j < (siz - 1))  n += printf_nice(',', USE_CHAR);
        else break;
      }

      n += printf_eol();
      pp += siz;
      i += siz;
    } else {
      return ECODE_PARAM;
    }
  }

  if (USE_CODEDUMP == xmode) {
    n += printf_text("};", USE_LT | USE_EOL);
    n += printf_text("unsigned int shell_code_size =", USE_LT);
    n += printf_nice(size, USE_DEC | USE_SEMICOLON | USE_EOL);
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
    imode_t s = mode & USE_NOSPACE ? USE_NONE : USE_SPACE;
    for (pconvert_t x = p; 0 != x->text; ++x) {
      if ((x->type & mask) == x->type) {
        n += printf_work(data + n, sizeof(data) - n, x->text, (mode & ~USE_EOL) | s);
        v &= ~x->type;
      }
    }

    if (v) {
      n += printf_neat(data + n, sizeof(data) - n, v, USE_UNKNOWN | USE_SPACE | (mode & ~USE_EOL));
    }

    n += printf_text(data, mode);
  } else if (mode & USE_EOL) {
    n += printf_eol();
  }

  return n;
}

int printf_maskmute(const pconvert_t p, const maskz_t mask, const imode_t mode) {
  int n = 0;
  if (p) {
    for (pconvert_t x = p; 0 != x->text; ++x) {
      if ((x->type & mask) == x->type) {
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
    return fprintf(STDOUT, "%s: DEBUG: %s\n\n", errname, data);
  }

  return fprintf(STDOUT, "DEBUG: %s\n\n", data);
}

int printf_e(const char* format, ...) {
  MALLOCA(char, data, MAX_BUFFER_SIZE);

  va_list pVAList;
  va_start(pVAList, format);
  vsnprintf(data, sizeof(data), format, pVAList);
  va_end(pVAList);

  if (errname[0]) {
    return fprintf(STDERR, "%s: ERROR: %s\n\n", errname, data);
  }

  return fprintf(STDERR, "ERROR: %s\n\n", data);
}

int printf_i(const char* format, ...) {
  MALLOCA(char, data, MAX_BUFFER_SIZE);

  va_list pVAList;
  va_start(pVAList, format);
  vsnprintf(data, sizeof(data), format, pVAList);
  va_end(pVAList);

  if (errname[0]) {
    return fprintf(STDOUT, "%s: INFO: %s\n\n", errname, data);
  }

  return fprintf(STDOUT, "INFO: %s\n\n", data);
}

int printf_w(const char* format, ...) {
  MALLOCA(char, data, MAX_BUFFER_SIZE);

  va_list pVAList;
  va_start(pVAList, format);
  vsnprintf(data, sizeof(data), format, pVAList);
  va_end(pVAList);

  if (errname[0]) {
    return fprintf(STDOUT, "%s: WARNING: %s\n\n", errname, data);
  }

  return fprintf(STDOUT, "WARNING: %s\n\n", data);
}

void printf_x(const char* format, ...) {
  MALLOCA(char, data, MAX_BUFFER_SIZE);

  va_list pVAList;
  va_start(pVAList, format);
  vsnprintf(data, sizeof(data), format, pVAList);
  va_end(pVAList);

  if (errname[0]) {
    fprintf(STDERR, "%s: FAIL: %s\n\n", errname, data);
  } else {
    fprintf(STDERR, "ERROR: %s\n\n", data);
  }

  exit(EXIT_FAILURE);
}

