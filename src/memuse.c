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

unknown_t xfree(unknown_t p) {
  if (p) {
    free(p);
  }

  return NULL;
}

char *xstrcpy(char *dst, const char *src) {
  if (dst && src) {
    return xstrncpy(dst, src, strlen(src));
  }

  return NULL;
}

char *xstrncpy(char *dst, const char *src, size_t count) {
  if (dst && src && count) {
    return strncpy(dst, src, count);
  }

  return NULL;
}

char* xstrdup(const char *src) {
  if (src) {
    return xstrndup(src, strlen(src));
  }

  return NULL;
}

char* xstrndup(const char *src, size_t size) {
  if (src && size) {
    char*  p = xmalloc(size);
    return xstrncpy(p, src, size);
  }

  return NULL;
}

unknown_t zfree(punknown_t p) {
  if (p && *p) {
    xfree(*p);
    *p = NULL;
  }

  return NULL;
}

