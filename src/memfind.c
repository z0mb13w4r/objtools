#include "memfind.h"

int isfind(handle_t p) {
  return ismodeNNN(p, MODE_FIND);
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

size_t fgetsize(handle_t p) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0 && p0->item) {
      return p0->epos - p0->cpos + 1;
    }
  }

  return 0;
}

unknown_t fgetp(handle_t p, const size_t chunksize) {
  unknown_t p0 = fget(p);
  if (p0) {
    pfind_t p1 = CAST(pfind_t, p);
    if (p1) {
      p1->cpos += chunksize;

      if (p1->cpos < p1->epos) return p0;
      p1->item = NULL;
    }
  }

  return NULL;
}

int64_t fgets8(handle_t p) {
  unknown_t p0 = fgetp(p, sizeof(int8_t));
  return p0 ? *CAST(int8_t*, p0) : 0;
}

int64_t fgets16(handle_t p) {
  unknown_t p0 = fgetp(p, sizeof(int16_t));
  return p0 ? *CAST(int16_t*, p0) : 0;
}

int64_t fgets32(handle_t p) {
  unknown_t p0 = fgetp(p, sizeof(int32_t));
  return p0 ? *CAST(int32_t*, p0) : 0;
}

int64_t fgets64(handle_t p) {
  unknown_t p0 = fgetp(p, sizeof(int64_t));
  return p0 ? *CAST(int64_t*, p0) : 0;
}

uint64_t fgetu8(handle_t p) {
  unknown_t p0 = fgetp(p, sizeof(uint8_t));
  return p0 ? *CAST(uint8_t*, p0) : 0;
}

uint64_t fgetu16(handle_t p) {
  unknown_t p0 = fgetp(p, sizeof(uint16_t));
  return p0 ? *CAST(uint16_t*, p0) : 0;
}

uint64_t fgetu32(handle_t p) {
  unknown_t p0 = fgetp(p, sizeof(uint32_t));
  return p0 ? *CAST(uint32_t*, p0) : 0;
}

uint64_t fgetu64(handle_t p) {
  unknown_t p0 = fgetp(p, sizeof(uint64_t));
  return p0 ? *CAST(uint64_t*, p0) : 0;
}

uint64_t fgetuleb128(handle_t p) {
  uint64_t n = 0;

  if (isfind(p)) {
    uint64_t y = 0;
    unknown_t p0 = fgetp(p, sizeof(uint8_t));
    while (p0) {
      uchar_t x = *CAST(uint8_t*, p0);
      n |= (x & 0x7f) << y;
      y += 7;
      if ((x & 0x80) == 0) break;
      p0 = fgetp(p, sizeof(uint8_t));
    }
  }

  return n;
}

char* fgetstring(handle_t p) {
  if (isfind(p)) {
    char* p0 = fget(p);
    while (fgets8(p));
    fgets8(p); // skip null terminator

    return p0;
  }

  return NULL;
}

handle_t fnext(handle_t p) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos += p0->chunksize;
      if (p0->cpos < p0->epos) return p0;
      p0->item = NULL;
    }

    return p0;
  }

  return NULL;
}

unknown_t fmove(handle_t p, const size_t cpos) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos = cpos;
      if (p0->cpos < p0->epos) return fget(p);
      p0->item = NULL;
    }
  }

  return NULL;
}

unknown_t fstep(handle_t p, const size_t chunksize) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos += chunksize;
      if (p0->cpos < p0->epos) return fget(p);
      p0->item = NULL;
    }
  }

  return NULL;
}

unknown_t fupdate(handle_t p, const size_t cpos, const size_t chunksize) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos = cpos;
      p0->chunksize = chunksize;
      if (p0->cpos < p0->epos) return fget(p);
      p0->item = NULL;
    }
  }

  return NULL;
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

handle_t fcalloc(unknown_t data, const size_t size, const size_t chunksize) {
  if (data) {
    pfind_t p = xmalloc(sizeof(find_t));
    if (p) {
      p->cpos = 0;
      p->epos = size;
      p->item = cmalloc(data, size);
      p->chunksize = chunksize;
    }
    return setmode(p, MODE_FINDC);
  }

  return NULL;
}

handle_t ffree(handle_t p) {
  if (ismodeNNN(p, MODE_FIND)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (ismode(p, MODE_FINDC)) {
      xfree(p0->item);
    }
    xfree(p0);
    return NULL;
  }

  return p;
}

