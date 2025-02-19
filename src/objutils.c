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

const char* get_string(const pconvert_t p, const pick_t x) {
  const char* s = get_stringnull(p, x);
  if (NULL == s) {
    return get_stringunknown(x);
  }

  return s;
}

const char* get_stringnull(const pconvert_t p, const pick_t x) {
  for (pconvert_t pp = p; 0 != pp->text; ++pp) {
    if (pp->type == x) {
      return pp->text;
    }
  }

  return NULL;
}

const char* get_stringunknown(const pick_t x) {
  static char buff[32];
  snprintf(buff, sizeof (buff), "<unknown: %x>", x);
  return buff;
}

