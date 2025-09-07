#include <unistd.h>

#include "memuse.h"

unknown_t xmalloc(const size_t size) {
  unknown_t p = NULL;
  if (0 != size) {
    p = malloc(size);
    if (p) {
      xmemset(p, 0, size);
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

size_t xstrlen(const char *src) {
  return src ? strlen(src) : 0;
}

char *xstrcat(char *dst, const char *src) {
  return dst && src ? xstrncat(dst, src, xstrlen(src)) : NULL;
}

char *xstrncat(char *dst, const char *src, size_t count) {
  return dst && src && count ? strncat(dst, src, count) : NULL;
}

char *xstrcpy(char *dst, const char *src) {
  return dst && src ? xstrncpy(dst, src, xstrlen(src)) : NULL;
}

char *xstrncpy(char *dst, const char *src, size_t count) {
  return dst && src && count ? strncpy(dst, src, count) : NULL;
}

char* xstrdup(const char *src) {
  return src ? xstrndup(src, xstrlen(src)) : NULL;
}

char* xstrndup(const char *src, size_t size) {
  if (src && size) {
    char*  p = xmalloc(size);
    return xstrncpy(p, src, size);
  }

  return NULL;
}

unknown_t xmemcpy(unknown_t dst, const unknown_t src, size_t count) {
  return dst && src && count ? memcpy(dst, src, count) : NULL;
}

unknown_t xmemset(unknown_t ptr, int value, size_t count) {
  return ptr && count ? memset(ptr, value, count) : NULL;
}

unknown_t zfree(punknown_t p) {
  if (p && *p) {
    xfree(*p);
    *p = NULL;
  }

  return NULL;
}

const char* strshorten(const char* p) {
  if (p) {
    const char *p0 = strrchr(p, '/');
    return p0 ? p0 + 1 : p;
  }

  return p;
}

