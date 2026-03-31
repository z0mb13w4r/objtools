#include "memuse.h"
#include "objregex.h"

bool_t isre(handle_t p) {
  return ismode(p, MODE_REGEX);
}

handle_t rmalloc(const char *pattern) {
  pre_t p0 = xmalloc(sizeof(re_t), MODE_HEAP);
  if (p0) {
    p0->data = xmalloc(sizeof(regex_t), MODE_HEAP);

    if (regcomp(p0->data, pattern, REG_EXTENDED)) {
      return setmode(p0, MODE_REGEX);
    }

    xfree(p0->data);
    xfree(p0);
  }

  return NULL;
}

handle_t rfree(handle_t p) {
  if (isre(p)) {
    pre_t p0 = CAST(pre_t, p);
    if (p0->data) {
      regfree(p0->data);
      xfree(p0->data);
    }

    xfree(p0);
    return NULL;
  }

  return p;
}

