#include <stdio.h>
#include <string.h>

#include "objutils.h"

bool_t isbits(const imode_t x) {
  return x & (x -1);
}

bool_t isused(ppick_t p, const pick_t x) {
  if (p) {
    for ( ; PICK_END != *p; ++p) {
      if (x == *p) return TRUE;
    }
  }

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
  const size_t siz = strlen(src);
  if (3 <= siz && '0' == src[0] && ('x' == src[1] || 'X' == src[1])) {
    return strtol(src + 2, NULL, 16);
  } else if (2 <= siz && ('h' == src[siz - 1] || 'H' == src[siz - 1])) {
    return strtol(src, NULL, 16);
  } else if (2 <= siz && ('b' == src[siz - 1] || 'B' == src[siz - 1])) {
    return strtol(src, NULL, 2);
  }

  return atol(src);
}

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
  if (p) {
    for (pconvert_t pp = p; 0 != pp->text; ++pp) {
      if (pp->type == x) {
        return pp->text;
      }
    }
  }

  return NULL;
}

const char* strpickunknown(const pick_t x) {
  static char o[32];
  snprintf(o, sizeof(o), "<unknown: %x>", x);
  return o;
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

