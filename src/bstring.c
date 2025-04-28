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

