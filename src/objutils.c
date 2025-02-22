#include <stdio.h>
#include <string.h>
#include "objutils.h"

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
  for (pconvert_t pp = p; 0 != pp->text; ++pp) {
    if (pp->type == x) {
      return pp->text;
    }
  }

  return NULL;
}

const char* strpickunknown(const pick_t x) {
  static char buff[32];
  snprintf(buff, sizeof (buff), "<unknown: %x>", x);
  return buff;
}

size_t strlenpick(const pconvert_t p) {
  size_t siz = 0;

  for (pconvert_t pp = p; 0 != pp->text; ++pp) {
    siz = MAX(siz, strlen(pp->text));
  }

  return siz;
}

