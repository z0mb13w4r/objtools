#include "memfind.h"

bool_t isfind(handle_t p) {
  return ismodeNNN(p, MODE_FIND);
}

bool_t fiseof(handle_t p) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    return p0 && p0->item ? p0->cpos > p0->epos : TRUE;
  }

  return TRUE;
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

size_t fgetcpos(handle_t p) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    return p0 ? p0->cpos : 0;
  }

  return 0;
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

      if (p1->cpos <= p1->epos) return p0;
      if (ismode(p1, MODE_FIND)) p1->item = NULL;
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

    return p0;
  }

  return NULL;
}

handle_t fsetu8byoffset(handle_t p, const uint64_t offset, const int8_t v) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos = offset;

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = v;
        return p;
      }
    }
  }

  return NULL;
}

handle_t fsetu16byoffset(handle_t p, const uint64_t offset, const int16_t v) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos = offset;

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x00) & 0xff;
      }

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x08) & 0xff;
        return p;
      }
    }
  }

  return NULL;
}

handle_t fsetu32byoffset(handle_t p, const uint64_t offset, const int32_t v) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos = offset;

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x00) & 0xff;
      }

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x08) & 0xff;
      }

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x10) & 0xff;
      }

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x18) & 0xff;
        return p;
      }
    }
  }

  return NULL;
}

handle_t fsetu64byoffset(handle_t p, const uint64_t offset, const int64_t v) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos = offset;

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x00) & 0xff;
      }

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x08) & 0xff;
      }

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x10) & 0xff;
      }

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x18) & 0xff;
      }

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x20) & 0xff;
      }

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x28) & 0xff;
      }

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x30) & 0xff;
      }

      if (p0->item && p0->cpos <= p0->epos) {
        CAST(puchar_t, p0->item)[p0->cpos++] = (v >> 0x38) & 0xff;
        return p;
      }
    }
  }

  return NULL;
}

handle_t fnext(handle_t p) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos += p0->chunksize;
      if (p0->cpos <= p0->epos) return p0;
      if (ismode(p0, MODE_FIND)) p0->item = NULL;
    }

    return p0;
  }

  return NULL;
}

handle_t freset(handle_t p) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos = 0;
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

      if (p0->cpos <= p0->epos) return fget(p);
      if (ismode(p0, MODE_FIND)) p0->item = NULL;
    }
  }

  return NULL;
}

handle_t fstep(handle_t p, const size_t chunksize) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos += chunksize;
      if (p0->cpos <= p0->epos) return p0;
      if (ismode(p0, MODE_FIND)) p0->item = NULL;
    }

    return p0;
  }

  return NULL;
}

unknown_t fupdate(handle_t p, const size_t cpos, const size_t chunksize) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0) {
      p0->cpos = cpos;
      p0->chunksize = chunksize;
      if (p0->cpos <= p0->epos) return fget(p);
      if (ismode(p0, MODE_FIND)) p0->item = NULL;
    }
  }

  return NULL;
}

handle_t fcalloc(unknown_t p, const size_t size, const size_t chunksize) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0 && p0->item) {
      handle_t p1 = fcalloc(CAST(puchar_t, p0->item) + p0->cpos, size, chunksize);
      p0->cpos += size;
      return p1;
    }
  } else if (p) {
    pfind_t p0 = xmalloc(sizeof(find_t));
    if (p0) {
      p0->cpos = 0;
      p0->epos = size - 1;
      p0->size = size;
      p0->item = cmalloc(p, size);
      p0->chunksize = chunksize;
    }
    return setmode(p0, MODE_FINDC);
  }

  return NULL;
}

handle_t fmalloc(unknown_t p, const size_t size, const size_t chunksize) {
  if (isfind(p)) {
    pfind_t p0 = CAST(pfind_t, p);
    if (p0 && p0->item) {
      handle_t p1 = fmalloc(CAST(puchar_t, p0->item) + p0->cpos, size, chunksize);
      p0->cpos += size;
      return p1;
    }
  } else if (p) {
    pfind_t p0 = xmalloc(sizeof(find_t));
    if (p0) {
      p0->cpos = 0;
      p0->epos = size - 1;
      p0->size = size;
      p0->item = p;
      p0->chunksize = chunksize;
    }
    return setmode(p0, MODE_FIND);
  }

  return NULL;
}

handle_t fxalloc(const size_t size, const size_t chunksize) {
  pfind_t p0 = xmalloc(sizeof(find_t));
  if (p0) {
    p0->cpos = 0;
    p0->epos = size - 1;
    p0->size = size;
    p0->item = xmalloc(size);
    p0->chunksize = chunksize;
  }
  return setmode(p0, MODE_FINDC);
}

handle_t fswap(handle_t p, handle_t q) {
  if (isfind(p) && isfind(q)) {
    pfind_t p0 = CAST(pfind_t, p);
    pfind_t q0 = CAST(pfind_t, q);

    xfree(p0->item);

    p0->cpos = q0->cpos;
    p0->epos = q0->epos;
    p0->size = q0->size;
    p0->item = q0->item;
    p0->chunksize = q0->chunksize;

    xfree(q0);

    return p0;
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

