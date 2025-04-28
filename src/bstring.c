#include <string.h>

#include "buffer.h"
#include "bstring.h"

bool_t isbstring(handle_t p) {
  return ismode(p, MODE_BSTRING) && 0 != CAST(pbstring_t, p)->data;
}


handle_t bstrmalloc() {
  handle_t p = xmalloc(sizeof(bstring_t));
  return setmode(p, MODE_BSTRING);
}

handle_t bstrmallocsize(const size_t size) {
  handle_t p = bstrmalloc();
  if (ismode(p, MODE_BSTRING)) {
    pbstring_t p0 = CAST(pbstring_t, p);
    p0->data = xmalloc(size);
    p0->size = size;
  }

  return p;
}

handle_t bstrfree(handle_t p) {
  if (ismode(p, MODE_BSTRING)) {
    free(CAST(pbstring_t, p)->data);
    free(p);
    return NULL;
  }

  return p;
}

handle_t bstring1(handle_t p) {
  if (isbstring(p)) {
    return bstring2(p, 0, CAST(pbstring_t, p)->size);
  } else if (isbuffer(p)) {
    return bstring2(p, 0, CAST(pbuffer_t, p)->size);
  }

  return NULL;
}

handle_t bstring2(handle_t p, const int offset, const size_t size) {
  if (isbstring(p)) {
    pbstring_t p0 = CAST(pbstring_t, p);
    if ((offset + size - 1) < p0->size) {
      pbstring_t p1 = bstrmallocsize(size);
      if (p1) {
        memcpy(p1->data, CAST(puchar_t, p0->data) + offset, size);
      }

      return p1;
    }
  } else if (isbuffer(p)) {
    pbuffer_t p0 = CAST(pbuffer_t, p);
    if ((offset + size - 1) < p0->size) {
      pbstring_t p1 = bstrmallocsize(size);
      if (p1) {
        memcpy(p1->data, CAST(puchar_t, p0->data) + offset, size);
      }

      return p1;
    }
  }

  return NULL;
}

handle_t bstring4(handle_t dst, handle_t src) {
  if (isbstring(dst) && isbstring(src)) {
    pbstring_t dst0 = CAST(pbstring_t, dst);
    free(dst0->data);

    pbstring_t src0 = CAST(pbstring_t, src);
    dst0->data = src0->data;
    free(src0);
    return dst;
  }

  return NULL;

}

size_t bstrlen(handle_t p) {
  if (isbstring(p)) {
    pbstring_t p0 = CAST(pbstring_t, p);
    return strnlen(p0->data, p0->size);
  }

  return 0;
}

size_t bstrsize(handle_t p) {
  return isbstring(p) ? CAST(pbstring_t, p)->size : 0;
}

handle_t bstrcpy(handle_t dst, handle_t src) {
  if (isbstring(dst) && isbstring(src)) {
    return bstrncpy(dst, src, CAST(pbstring_t, dst)->size);
  }

  return NULL;
}

handle_t bstrncpy(handle_t dst, handle_t src, size_t size) {
  if (isbstring(dst) && isbstring(src)) {
    pbstring_t dst0 = CAST(pbstring_t, dst);
    pbstring_t src0 = CAST(pbstring_t, src);
    if (size <= dst0->size) {
      strncpy(dst0->data, src0->data, size);
      return dst;
    }
  }

  return NULL;
}

handle_t bstrcat(handle_t dst, handle_t src) {
  if (isbstring(dst) && isbstring(src)) {
    return bstrncat(dst, src, CAST(pbstring_t, dst)->size);
  }

  return NULL;
}

handle_t bstrncat(handle_t dst, handle_t src, size_t size) {
  if (isbstring(dst) && isbstring(src)) {
    pbstring_t dst0 = CAST(pbstring_t, dst);
    pbstring_t src0 = CAST(pbstring_t, src);
    if (size <= dst0->size) {
      strncat(dst0->data, src0->data, size);
      return dst;
    }
  }

  return NULL;
}

handle_t bstrclr(handle_t p) {
  if (isbstring(p)) {
    pbstring_t p0 = CAST(pbstring_t, p);
    memset(p0->data, 0, p0->size);
    return p;
  }

  return NULL;
}

handle_t bstrlwr(handle_t p) {
  if (isbstring(p)) {
    pbstring_t p0 = CAST(pbstring_t, p);
    puchar_t p1 = CAST(puchar_t, p0->data);
    for (size_t i = 0; i < p0->size; ++i) {
      if (0 == p1[i]) break;
      else if ('A' <= p1[i] && p1[i] <= 'Z') p1[i] += 32;
    }

    return p;
  }

  return NULL;
}

handle_t bstrupr(handle_t p) {
  if (isbstring(p)) {
    pbstring_t p0 = CAST(pbstring_t, p);
    puchar_t p1 = CAST(puchar_t, p0->data);
    for (size_t i = 0; i < p0->size; ++i) {
      if (0 == p1[i]) break;
      else if ('a' <= p1[i] && p1[i] <= 'z') p1[i] -= 32;
    }

    return p;
  }

  return NULL;
}

int bstrcmp(handle_t s1, handle_t s2) {
  if (isbstring(s1) && isbstring(s2)) {
    pbstring_t s10 = CAST(pbstring_t, s1);
    pbstring_t s20 = CAST(pbstring_t, s2);
    return strncmp(s10->data, s20->data, MIN(s10->size, s20->size));
  }

  return -1;
}

int bstrncmp(handle_t s1, handle_t s2, size_t size) {
  if (isbstring(s1) && isbstring(s2)) {
    pbstring_t s10 = CAST(pbstring_t, s1);
    pbstring_t s20 = CAST(pbstring_t, s2);
    return strncmp(s10->data, s20->data, size);
  }

  return -1;
}

int bstrcasecmp(handle_t s1, handle_t s2) {
  if (isbstring(s1) && isbstring(s2)) {
    pbstring_t s10 = CAST(pbstring_t, s1);
    pbstring_t s20 = CAST(pbstring_t, s2);
    return strncasecmp(s10->data, s20->data, MIN(s10->size, s20->size));
  }

  return -1;
}

int bstrncasecmp(handle_t s1, handle_t s2, size_t size) {
  if (isbstring(s1) && isbstring(s2)) {
    pbstring_t s10 = CAST(pbstring_t, s1);
    pbstring_t s20 = CAST(pbstring_t, s2);
    return strncasecmp(s10->data, s20->data, size);
  }

  return -1;
}

