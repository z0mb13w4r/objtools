#include <unistd.h>

#include "crc.h"
#include "memuse.h"

bool_t ismode0(handle_t p, const nmode_t mode) {
  if (p) {
    const puchar_t p0 = CAST(puchar_t, p);
    if (MODE_GET0(mode) != p0[0])      return FALSE;
    return TRUE;
  }

  return FALSE;
}

bool_t ismode1(handle_t p, const nmode_t mode) {
  if (p) {
    const puchar_t p0 = CAST(puchar_t, p);
    if (MODE_GET1(mode) != p0[1])      return FALSE;
    return TRUE;
  }

  return FALSE;
}

bool_t ismode2(handle_t p, const nmode_t mode) {
  if (p) {
    const puchar_t p0 = CAST(puchar_t, p);
    if (MODE_GET2(mode) != p0[2])      return FALSE;
    return TRUE;
  }

  return FALSE;
}

bool_t ismode3(handle_t p, const nmode_t mode) {
  if (p) {
    const puchar_t p0 = CAST(puchar_t, p);
    if (MODE_GET3(mode) != p0[3])      return FALSE;
    return TRUE;
  }

  return FALSE;
}

bool_t ismodeNNN(handle_t p, const nmode_t mode) {
  return ismode0(p, mode) && ismode1(p, mode) && ismode2(p, mode);
}

bool_t ismodeNXXN(handle_t p, const nmode_t mode) {
  return ismode0(p, mode) && ismode3(p, mode);
}

bool_t ismode(handle_t p, const nmode_t mode) {
  return ismodeNNN(p, mode) && ismode3(p, mode);
}

handle_t setmode(handle_t p, const nmode_t mode) {
  if (p) {
    const puchar_t p0 = CAST(puchar_t, p);
    p0[0] = MODE_GET0(mode);
    p0[1] = MODE_GET1(mode);
    p0[2] = MODE_GET2(mode);
    p0[3] = MODE_GET3(mode);
  }

  return p;
}

unknown_t cmalloc(const unknown_t p, const size_t size, const nmode_t mode) {
  if (p) {
    unknown_t p0 = xmalloc(size, mode);
    xmemcpy(p0, p, size);
    return p0;
  }

  return NULL;
}

unknown_t xmalloc(const size_t size, const nmode_t mode) {
  if (0 != size) {
    const size_t maxsize = size + sizeof(memuse_t);
    pmemuse_t p = NULL;

    if (MODE_HEAP == mode) {
      p = malloc(maxsize);
    }

    if (p) {
      xmemclr(p, maxsize);
      setmode(p, mode);
      p->size = size;
      p->sizemax = size;
      return p + 1;
    }
  }

  return NULL;
}

unknown_t xfree(unknown_t p) {
  if (p) {
    pmemuse_t p0 = CAST(pmemuse_t, p) - 1;
    if (ismode(p0, MODE_HEAP)) {
      free(p0);
    } else if (!ismode(p0, MODE_STACK)) {
      free(p);
    }
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

