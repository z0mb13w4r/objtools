#include <unistd.h>

#include "crc.h"
#include "memuse.h"

unknown_t cmalloc(const unknown_t p, const size_t size, const nmode_t mode) {
  if (p) {
    unknown_t p0 = xmalloc(size, mode);
    xmemcpy(p0, p, size);
    return p0;
  }

  return NULL;
}

unknown_t xmalloc(const size_t size, const nmode_t mode) {
  unknown_t p = NULL;
  if (0 != size) {
    if (MODE_HEAP == mode) {
      p = malloc(size);
    } else if (MODE_STACK == mode) {
      p = alloca(size);
    }
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

unknown_t zfree(punknown_t p) {
  if (p && *p) {
    xfree(*p);
    *p = NULL;
  }

  return NULL;
}

int xstrcmp(const char *x, const char *y) {
  return x && y ? strcmp(x, y) : INT_MAX;
}

int xstrncmp(const char *x, const char *y, size_t count) {
  return x && y ? strncmp(x, y, count) : INT_MAX;
}

int xstrichr(const char *str, int ch) {
  if (ch) {
    char *p = xstrchr(str, ch);
    return p ? p - str : -1;
  }

  return xstrlen(str);
}

size_t xstrlen(const char *s) {
  return s ? strlen(s) : 0;
}

size_t xstrnlen(const char *s, size_t count) {
  return s ? strnlen(s, count) : 0;
}

char *xstrchr(const char* str, int ch) {
  return str ? strchr(str, ch) : NULL;
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

char* xstrdup(const char *str) {
  return str ? xstrndup(str, xstrlen(str) + 1) : NULL;
}

char* xstrndup(const char *str, size_t size) {
  if (str && size) {
    char*  p = xmalloc(size, MODE_HEAP);
    return xstrncpy(p, str, size);
  }

  return NULL;
}

char* xstrgrab(const char* name, int spos, int epos) {
  MALLOCA(char, ii, 2 * 1024);

  if (name && spos && epos) {
    FILE *fp = fopen(name, "rt");
    if (fp) {
      int i = 0;
      int n = 0;
      while (fgets(ii + n, sizeof(ii) - n, fp)) {
        ++i;
        if (spos <= i && i < epos) {
          n = xstrlen(ii);
        } else if (epos == i) {
          break;
        }
      }

      fclose(fp);
      return xstrdup(ii);
    }
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

const char* strshorten(const char* p) {
  if (p) {
    const char *p0 = strrchr(p, '/');
    return p0 ? p0 + 1 : p;
  }

  return p;
}

