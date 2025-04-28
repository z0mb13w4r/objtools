#include "buffer.h"
#include "bstring.h"

handle_t bstrmalloc() {
  handle_t p = xmalloc(sizeof(bstring_t));
  return setmode(p, MODE_STRING);
}

handle_t bstrmallocsize(const size_t size) {
  handle_t p = bstrmalloc();
  if (ismode(p, MODE_STRING)) {
    pbstring_t p0 = CAST(pbstring_t, p);
    p0->data = xmalloc(size);
    p0->size = size;
  }

  return p;
}

handle_t bstrfree(handle_t p) {
  if (ismode(p, MODE_STRING)) {
    free(CAST(pbstring_t, p)->data);
    free(p);
    return NULL;
  }

  return p;
}

