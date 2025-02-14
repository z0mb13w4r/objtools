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
  static char buff[32];

  for (pconvert_t x = p; 0 != x->text; ++x) {
    if (x->type == type) {
      return x->text;
    }
  }

  snprintf(buff, sizeof (buff), "<unknown: %x>", type);
  return buff;
}

