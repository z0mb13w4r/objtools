#include "memuse.h"

unknown_t xmalloc(const size_t size) {
  unknown_t p = NULL;
  if (0 != size) {
    p = malloc(size);
    if (p) {
      memset(p, 0, size);
    }
  }

  return p;
}

unknown_t zfree(punknown_t p) {
  if (p && *p) {
    free(*p);
    *p = NULL;
  }

  return NULL;
}

