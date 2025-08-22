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

char* xstrdup(const char *s) {
  if (s) {
    return xstrndup(s, strlen(s));
  }

  return NULL;
}

char* xstrndup(const char *s, size_t size) {
  if (s && size) {
    char*  p = xmalloc(size);
    return strncpy(p, s, size);
  }

  return NULL;
}

unknown_t zfree(punknown_t p) {
  if (p && *p) {
    free(*p);
    *p = NULL;
  }

  return NULL;
}

