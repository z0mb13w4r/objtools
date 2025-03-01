#include "memfind.h"

int isfind(handle_t p) {
  if (p) {
    return ismode(p, MODE_FIND);
  }

  return 0;
}

void* fget(handle_t p) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0 && p0->item) {
      return CAST(unsigned char*, p0->item) + p0->cpos;
    }
  }

  return NULL;
}

handle_t fnext(handle_t p) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos += p0->chunksize;
      if (p0->cpos < p0->epos) return p;
      p0->item = NULL;
    }
  }

  return p;
}

handle_t fmalloc(void *data, const size_t size, const size_t chunksize) {
  if (data) {
    pfind_t p = mallocx(sizeof(find_t));
    if (p) {
      p->cpos = 0;
      p->epos = size;
      p->item = data;
      p->chunksize = chunksize;
    }
    return setmode(p, MODE_FIND);
  }

  return NULL;
}

handle_t ffree(handle_t p) {
  if (isfind(p)) {
    free(p);
    return NULL;
  }

  return p;
}

