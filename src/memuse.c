#include <unistd.h>

#include "crc.h"
#include "memuse.h"

unknown_t xmalloc(const size_t size) {
  unknown_t p = NULL;
  if (0 != size) {
    p = malloc(size);
    if (p) {
      xmemclr(p, size);
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

int xstrcmp(const char *x, const char *y) {
  return x && y ? strcmp(x, y) : INT_MAX;
}

size_t xstrlen(const char *src) {
  return src ? strlen(src) : 0;
}

char *xstrcat(char *dst, const char *src) {
  return dst && src ? strcat(dst, src) : NULL;
}

char *xstrncat(char *dst, const char *src, size_t count) {
  return dst && src && count ? strncat(dst, src, count) : NULL;
}

char *xstrcpy(char *dst, const char *src) {
  return dst && src ? strcpy(dst, src) : NULL;
}

char *xstrncpy(char *dst, const char *src, size_t count) {
  return dst && src && count ? strncpy(dst, src, count) : NULL;
}

char* xstrdup(const char *src) {
  return src ? xstrndup(src, xstrlen(src) + 1) : NULL;
}

char* xstrndup(const char *src, size_t size) {
  if (src && size) {
    char*  p = xmalloc(size);
    return xstrncpy(p, src, size);
  }

  return NULL;
}

uchar_t xstrcrc8(const char* src) {
  return xstrncrc8(src, xstrlen(src));
}

uchar_t xstrncrc8(const char* src, size_t count) {
  return crc8_calculate(CRC_DEF8, CAST(puchar_t, src), count);
}

ushort_t xstrcrc16(const char* src) {
  return xstrncrc16(src, xstrlen(src));
}

ushort_t xstrncrc16(const char* src, size_t count) {
  return crc16_calculate(CRC_DEF16, CAST(puchar_t, src), count);
}

ulong_t xstrcrc32(const char* src) {
  return xstrncrc32(src, xstrlen(src));
}

ulong_t xstrncrc32(const char* src, size_t count) {
  return crc32_calculate(CRC_DEF32, CAST(puchar_t, src), count);
}

unknown_t xmemcpy(unknown_t dst, const unknown_t src, size_t count) {
  return dst && src && count ? memcpy(dst, src, count) : NULL;
}

unknown_t xmemset(unknown_t ptr, int value, size_t count) {
  return ptr && count ? memset(ptr, value, count) : NULL;
}

unknown_t xmemclr(unknown_t ptr, size_t count) {
  return xmemset(ptr, 0, count);
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

