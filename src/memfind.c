#include "memfind.h"

int isfind(handle_t p) {
  if (p) {
    return ismode(p, MODE_FIND);
  }

  return 0;
}

unknown_t fget(handle_t p) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0 && p0->item) {
      return CAST(puchar_t, p0->item) + p0->cpos;
    }
  }

  return NULL;
}

handle_t fnext(handle_t p) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    return p0 ? fstep(p, p0->chunksize) : NULL;
  }

  return p;
}

handle_t fmove(handle_t p, const size_t cpos) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos = cpos;
      if (p0->cpos < p0->epos) return p;
      p0->item = NULL;
    }
  }

  return p;
}

handle_t fstep(handle_t p, const size_t chunksize) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos += chunksize;
      if (p0->cpos < p0->epos) return p;
      p0->item = NULL;
    }
  }

  return p;
}

handle_t fupdate(handle_t p, const size_t cpos, const size_t chunksize) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos = cpos;
      p0->chunksize = chunksize;
      if (p0->cpos < p0->epos) return p;
      p0->item = NULL;
    }
  }

  return p;
}

handle_t fmalloc(unknown_t data, const size_t size, const size_t chunksize) {
  if (data) {
    pfind_t p = xmalloc(sizeof(find_t));
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
    xfree(p);
    return NULL;
  }

  return p;
}

