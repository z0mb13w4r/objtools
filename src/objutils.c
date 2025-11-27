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

uint64_t atoimode(const char* src) {
  return atovalue(src);
}

uint64_t atovalue(const char* src) {
  const size_t siz = xstrlen(src);
  if (3 <= siz && '0' == src[0] && (('x' | 0x20) == src[1])) {
    return strtol(src + 2, NULL, 16);
  } else if (3 <= siz && (('b' | 0x20) == src[siz - 1]) && (('k' | 0x20) == src[siz - 2])) {
    return atol(src) * 1024;
  } else if (3 <= siz && (('b' | 0x20) == src[siz - 1]) && (('m' | 0x20) == src[siz - 2])) {
    return atol(src) * 1024 * 1024;
  } else if (3 <= siz && (('b' | 0x20) == src[siz - 1]) && (('g' | 0x20) == src[siz - 2])) {
    return atol(src) * 1024 * 1024 * 1024;
  } else if (3 <= siz && (('b' | 0x20) == src[siz - 1]) && (('t' | 0x20) == src[siz - 2])) {
    return atol(src) * 1024 * 1024 * 1024 * 1024;
  } else if (2 <= siz && (('h' | 0x20) == src[siz - 1])) {
    return strtol(src, NULL, 16);
  } else if (2 <= siz && (('b' | 0x20) == src[siz - 1])) {
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
  static char o[32];
  printf_neat(o, sizeof(o), x, USE_UNKNOWN);
  return o;
}

size_t strlenpick(const pconvert_t p) {
  size_t siz = 0;

  for (pconvert_t pp = p; 0 != pp->text; ++pp) {
    siz = MAX(siz, xstrlen(pp->text));
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
  return '0' <= x && x <= '9' ? TRUE : FALSE;
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
  if ('a' <= x && x <= 'f') return TRUE;
  else if ('A' <= x && x <= 'F') return TRUE;
  return isdec8(x);
}

bool_t ishexb(unknown_t p, const size_t size) {
  if (p && 0 != size) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if (!ishex8(p0[i])) return FALSE;
    }

    return TRUE;
  }

  return FALSE;
}

int64_t bin8(int x) {
  return '0' <= x && x <= '1' ? x - '0' : 0;
}

int64_t binb(unknown_t p, const size_t size) {
  int64_t x = 0;
  if (p && 0 != size) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if (!isbin8(p0[i])) break;
      x = (x << 1) | bin8(p0[i]);
    }
  }

  return x;
}

int64_t dec8(int x) {
  return '0' <= x && x <= '9' ? x - '0' : 0;
}

int64_t decb(unknown_t p, const size_t size) {
  int64_t x = 0;
  if (p && 0 != size) {
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
  if (p && 0 != size) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size; ++i) {
      if (!ishex8(p0[i])) break;
      x = (x << 4) | hex8(p0[i]);
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

int strcmp8(const unknown_t s0, const char* s1, const size_t maxsize) {
  if (s0 && s1) {
    puchar_t p0 = CAST(puchar_t, s0);
    puchar_t p1 = CAST(puchar_t, s1);
    for (size_t i = 0; i < maxsize; ++i, ++p0, ++p1) {
      const uchar_t c0 = *p0;
      const uchar_t c1 = *p1;
      if (0 == c0 || 0 == c1 || c0 != c1) return c0 - c1;
    }

    return ECODE_OK;
  }

  return ECODE_NULL;
}

int strcmp16(const unknown_t s0, const char* s1, const size_t maxsize) {
  if (s0 && s1) {
    pushort_t p0 = CAST(pushort_t, s0);
    puchar_t  p1 = CAST(puchar_t, s1);
    for (size_t i = 0; i < maxsize; i += 2, ++p0, ++p1) {
      const uchar_t c0 = *p0 & 0xff;
      const uchar_t c1 = *p1;
      if (0 == c0 || 0 == c1 || c0 != c1) return c0 - c1;
    }

    return ECODE_OK;
  }

  return ECODE_NULL;
}

