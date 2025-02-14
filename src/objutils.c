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

const char* get_string(const pconvert_t p, const unsigned int type) {
  const char* s = get_stringnull(p, type);
  if (s) return s;

  static char buff[32];
  snprintf(buff, sizeof (buff), "<unknown: %x>", type);
  return buff;
}

const char* get_stringnull(const pconvert_t p, const unsigned int type) {
  for (pconvert_t x = p; 0 != x->text; ++x) {
    if (x->type == type) {
      return x->text;
    }
  }

  return NULL;
}

