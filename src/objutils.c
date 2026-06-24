#include <stdio.h>
#include <string.h>

#include "memuse.h"
#include "printf.h"
#include "objutils.h"

int countbits(const imode_t x) {
  imode_t v = x;
  int c = 0;

  while (v) {
    c += v & 1;
    v >>= 1;
  }

  return c;
}

bool_t isbits(const imode_t x) {
  return countbits(x) < 2 ? FALSE : TRUE;
}

bool_t isused(ppick_t p, const pick_t x) {
  if (p) {
    for ( ; PICK_END != *p; ++p) {
      if (x == *p) return TRUE;
    }
  }

  return FALSE;
}

bool_t isbool(const char* p, const size_t size) {
static char* zBOOLS[] = {
  "TRUE", "True", "true",
  "ON", "On", "on",
  "T", "t",
  "Y", "y",
  "1", 0
};

  for (size_t i = 0; zBOOLS[i]; ++i) {
    if (0 == xstrncmp(zBOOLS[i], p, size)) {
      return TRUE;
    }
  }

  return FALSE;
}

bool_t isnamedone(const char* names[], const size_t maxnames, const char* check) {
  size_t i = 0;
  for (i = 0; i < maxnames && names[i]; ++i) {
    if (0 == xstrcmp(names[i], check)) return TRUE;
  }

  if (i < maxnames) names[i] = check;

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

uint64_t atovalue(const char* src) {
  const size_t siz = xstrlen(src);
  if (3 <= siz && '0' == src[0] && ('x' == LOWER8(src[1]))) {
    return strtol(src + 2, NULL, 16);
  } else if (3 <= siz && ('b' == LOWER8(src[siz - 1])) && ('k' == LOWER8(src[siz - 2]))) {
    return atol(src) * 1024;
  } else if (3 <= siz && ('b' == LOWER8(src[siz - 1])) && ('m' == LOWER8(src[siz - 2]))) {
    return atol(src) * 1024 * 1024;
  } else if (3 <= siz && ('b' == LOWER8(src[siz - 1])) && ('g' == LOWER8(src[siz - 2]))) {
    return atol(src) * 1024 * 1024 * 1024;
  } else if (3 <= siz && ('b' == LOWER8(src[siz - 1])) && ('t' == LOWER8(src[siz - 2]))) {
    return atol(src) * 1024 * 1024 * 1024 * 1024;
  } else if (2 <= siz && ('h' == LOWER8(src[siz - 1]))) {
    return strtol(src, NULL, 16);
  } else if (2 <= siz && ('b' == LOWER8(src[siz - 1]))) {
    return strtol(src, NULL, 2);
  }

  return atol(src);
}

pconvert_t convertpicknull(const pconvert_t p, const pick_t x) {
  if (p) {
    for (pconvert_t pp = p; 0 != pp->text; ++pp) {
      if (pp->type == x) {
        return pp;
      }
    }
  }

  return NULL;
}

char* strname(char* dst, const char* src) {
  const char *p = strshorten(src);
  if (p) {
    return xstrcpy(dst, p);
  }

  return xstrcpy(dst, src);
}

const char* strpick(const pconvert_t p, const pick_t x) {
  const char* s = strpicknull(p, x);
  if (NULL == s) {
    return strpickunknown(x);
  }

  return s;
}

const char* strpicknull(const pconvert_t p, const pick_t x) {
  pconvert_t p0 = convertpicknull(p, x);
  return p0 ? p0->text : NULL;
}

const char* strpickunknown(const pick_t x) {
  static char o[64];
  printf_neat(o, sizeof(o), x, USE_UNKNOWN);
  return o;
}

size_t strlenpick(const pconvert_t p) {
  size_t siz = 0;
  if (p) {
    for (pconvert_t pp = p; 0 != pp->text; ++pp) {
      siz = MAX(siz, xstrlen(pp->text));
    }
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

bool_t isbin8(int x) {
  return '0' == x || '1' == x ? TRUE : FALSE;
}

bool_t isbinb(unknown_t p, const size_t size) {
  if (p && 0 != size) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if (!isbin8(p0[i])) return FALSE;
    }

    return TRUE;
  }

  return FALSE;
}

bool_t isdec8(int x) {
  return ISSCOPE(x, '0', '9') ? TRUE : FALSE;
}

bool_t isdecb(unknown_t p, const size_t size) {
  if (p && 0 != size) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if (!isdec8(p0[i])) return FALSE;
    }

    return TRUE;
  }

  return FALSE;
}

bool_t ishex8(int x) {
  if (ISSCOPE(x, 'a', 'f')) return TRUE;
  else if (ISSCOPE(x, 'A', 'F')) return TRUE;
  return isdec8(x);
}

bool_t ishexb(unknown_t p, const size_t size) {
  if (p && ISSIZE(size)) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if (!ishex8(p0[i])) return FALSE;
    }

    return TRUE;
  }

  return FALSE;
}

bool_t isreal8(int x) {
  if ('.' == x) return TRUE;
  return isdec8(x);
}

bool_t isrealb(unknown_t p, const size_t size) {
  if (p && ISSIZE(size)) {
    size_t cnt = 0;
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if ('.' == p0[i]) {
        ++cnt;
        if (1 != cnt) return FALSE;
      } else if (!isreal8(p0[i])) {
        return FALSE;
      }
    }

    return TRUE;
  }

  return FALSE;
}

bool_t isspecial8(int x) {
  return '!' == x || '#' == x || '$' == x || '&' == x ||
         '*' == x || '+' == x || '-' == x || '.' == x ||
         ':' == x || ';' == x || '=' == x || '?' == x ||
         '@' == x || '^' == x || '_' == x || '|' == x ||
         '~' == x;
}

bool_t isspecialb(unknown_t p, const size_t size) {
  if (p && ISSIZE(size)) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if (!isspecial8(p0[i])) return FALSE;
    }

    return TRUE;
  }

  return FALSE;
}

bool_t isstr8(int x) {
  return ISSCOPE(x, 'A', 'Z') || ISSCOPE(x, 'a', 'z') || isspecial8(x) || isdec8(x);
}

bool_t isstrb(unknown_t p, const size_t size) {
  if (p && ISSIZE(size)) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if (!isstr8(p0[i])) return FALSE;
    }

    return TRUE;
  }

  return FALSE;
}

uint64_t bin8(int x) {
  return '0' <= x && x <= '1' ? x - '0' : 0;
}

uint64_t binb(unknown_t p, const size_t size) {
  uint64_t x = 0;
  if (p && ISSIZE(size)) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if (!isbin8(p0[i])) break;
      x = (x << 1) | bin8(p0[i]);
    }
  }

  return x;
}

uint64_t dec8(int x) {
  return '0' <= x && x <= '9' ? x - '0' : 0;
}

uint64_t decb(unknown_t p, const size_t size) {
  uint64_t x = 0;
  if (p && ISSIZE(size)) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if (!isdec8(p0[i])) break;
      x = (x * 10) + dec8(p0[i]);
    }
  }

  return x;
}

uint64_t hex8(int x) {
  if ('a' <= x && x <= 'f')      return x - 'a' + 10;
  else if ('A' <= x && x <= 'F') return x - 'A' + 10;
  return dec8(x);
}

uint64_t hex16(int x0, int x1) {
  return hex8(x0) << 8 | hex8(x1);
}

uint64_t hexb(unknown_t p, const size_t size) {
  uint64_t x = 0;
  if (p && ISSIZE(size)) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if (!ishex8(p0[i])) break;
      x = (x << 4) | hex8(p0[i]);
    }
  }

  return x;
}

float64_t real8(int x) {
  return '.' == x ? -1.0 : CAST(float64_t, dec8(x));
}

float64_t realb(unknown_t p, const size_t size) {
  float64_t x = 0.0;
  float64_t y = 0.0;
  float64_t z = 10.0;
  if (p && ISSIZE(size)) {
    size_t cnt = 0;
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if ('.' == p0[i]) {
        ++cnt;
        if (1 != cnt) break;
      } else if (!isreal8(p0[i])) {
        break;
      } else if (1 == cnt) {
        y = (y * 10.0) + real8(p0[i]);
        z =  z * 10.0;
      } else {
        x = (x * 10.0) + real8(p0[i]);
      }
    }

    if (y > 0.0) {
      x = x + (y / z);
    }
  }

  return x;
}

int strlen8(unknown_t s, const size_t maxsize) {
  if (s) {
    puchar_t s0 = CAST(puchar_t, s);
    for (size_t i = 0; i < maxsize; ++i, ++s0) {
      if (0 == *s0) return i;
    }

    return maxsize;
  }

  return ECODE_NULL;
}

int strlen16(unknown_t s, const size_t maxsize) {
  if (s) {
    pushort_t s0 = CAST(pushort_t, s);
    for (size_t i = 0; i < maxsize; ++i, ++s0) {
      if (0 == *s0) return i;
    }

    return maxsize;
  }

  return ECODE_NULL;
}

int strsize8(unknown_t s, const size_t maxsize) {
  if (s) {
    puchar_t s0 = CAST(puchar_t, s);
    for (size_t i = 0; i < maxsize; ++i, ++s0) {
      if (0 == *s0) return i + 1;
    }

    return maxsize;
  }

  return ECODE_NULL;
}

int strsize16(unknown_t s, const size_t maxsize) {
  if (s) {
    pushort_t s0 = CAST(pushort_t, s);
    for (size_t i = 0; i < maxsize; i += 2, ++s0) {
      if (0 == *s0) return i + 2;
    }

    return maxsize;
  }

  return ECODE_NULL;
}

int8_t endian_s8(const bool_t yes, const int8_t v) {
  return v;
}

int16_t endian_s16(const bool_t yes, const int16_t v) {
  if (yes) {
    return MODE_PUT1(MODE_GET0(v)) | MODE_PUT0(MODE_GET1(v));
  }

  return v;
}

int32_t endian_s32(const bool_t yes, const int32_t v) {
  if (yes) {
    return MODE_PUT3(MODE_GET0(v)) | MODE_PUT2(MODE_GET1(v))
         | MODE_PUT1(MODE_GET2(v)) | MODE_PUT0(MODE_GET3(v));
  }

  return v;
}

int64_t endian_s64(const bool_t yes, const int64_t v) {
  if (yes) {
    return MODE_PUT7(MODE_GET0(v)) | MODE_PUT6(MODE_GET1(v))
         | MODE_PUT5(MODE_GET2(v)) | MODE_PUT4(MODE_GET3(v))
         | MODE_PUT3(MODE_GET4(v)) | MODE_PUT2(MODE_GET5(v))
         | MODE_PUT1(MODE_GET6(v)) | MODE_PUT0(MODE_GET7(v));
  }

  return v;
}

uint8_t endian_u8(const bool_t yes, const uint8_t v) {
  return v;
}

uint16_t endian_u16(const bool_t yes, const uint16_t v) {
  if (yes) {
    return MODE_PUT1(MODE_GET0(v)) | MODE_PUT0(MODE_GET1(v));
  }

  return v;
}

uint32_t endian_u32(const bool_t yes, const uint32_t v) {
  if (yes) {
    return MODE_PUT3(MODE_GET0(v)) | MODE_PUT2(MODE_GET1(v))
         | MODE_PUT1(MODE_GET2(v)) | MODE_PUT0(MODE_GET3(v));
  }

  return v;
}

uint64_t endian_u64(const bool_t yes, const uint64_t v) {
  if (yes) {
    return MODE_PUT7(MODE_GET0(v)) | MODE_PUT6(MODE_GET1(v))
         | MODE_PUT5(MODE_GET2(v)) | MODE_PUT4(MODE_GET3(v))
         | MODE_PUT3(MODE_GET4(v)) | MODE_PUT2(MODE_GET5(v))
         | MODE_PUT1(MODE_GET6(v)) | MODE_PUT0(MODE_GET7(v));
  }

  return v;
}

