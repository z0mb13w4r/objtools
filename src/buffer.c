#include <string.h>
#include <malloc.h>

#include "buffer.h"
#include "printf.h"
#include "elfcode.h"
#include "options.h"
#include "memlink.h"
#include "objutils.h"

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

unknown_t xmalloc(const size_t size) {
  unknown_t p = malloc(size);
  if (p) {
    memset(p, 0, size);
  }
  return p;
}

unknown_t xdump(unknown_t p) {
  if (ismodeNNN(p, MODE_LINK)) {
    lfree(p);
  }

  return NULL;
}

unknown_t xfree(unknown_t p) {
  if (p) {
    if (ismodeNNN(p, MODE_BUFFER)) {
      return bfree(p);
    } else if (ismodeNNN(p, MODE_OPTIONS)) {
      return ofree(p);
    } else if (ismodeNNN(p, MODE_ACTIONS)) {
      return afree(p);
    } else if (ismodeNNN(p, MODE_LINK)) {
      xfree(CAST(pnode_t, p)->item);
      return lfree(p);
    }

    free(p);
  }

  return NULL;
}

size_t xread(unknown_t p, size_t size, size_t count, FILE *f) {
  if (p) {
    memset(p, 0, size * count);
    return fread(p, size, count, f);
  }

  return 0;
}

handle_t bmalloc() {
  handle_t p = xmalloc(sizeof(buffer_t));
  return setmode(p, MODE_BUFFER);
}

handle_t bmallocsize(const size_t size) {
  handle_t p = bmalloc();
  if (ismode(p, MODE_BUFFER)) {
    pbuffer_t p0 = CAST(pbuffer_t, p);
    p0->data = xmalloc(size);
    p0->size = size;
  }

  return p;
}

handle_t bfree(handle_t p) {
  if (ismode(p, MODE_BUFFER)) {
    free(CAST(pbuffer_t, p)->data);
    free(p);
    return NULL;
  }

  return p;
}

handle_t bclone(handle_t p, const int offset, const size_t size) {
  if (ismode(p, MODE_BUFFER)) {
    unknown_t p0 = getp(p, offset, size);
    if (p0) {
      pbuffer_t p1 = bmalloc(size);
      if (issafe(p1)) {
        memcpy(p1->data, p0, size);
        return p1;
      }
    }
  }

  return NULL;
}

handle_t bappend(handle_t p, unknown_t px, const size_t size) {
  if (ismode(p, MODE_BUFFER) && px) {
    size_t epos = CAST(pbuffer_t, p)->size;
    p = bresize(p, epos + size);
    if (ismode(p, MODE_BUFFER)) {
      pbuffer_t p0 = CAST(pbuffer_t, p);
      memcpy(CAST(puchar_t, p0->data) + epos, px, size);
    }
  }

  return p;
}

handle_t bresize(handle_t p, const size_t size) {
  if (ismode(p, MODE_BUFFER)) {
    pbuffer_t p0 = CAST(pbuffer_t, p);
    unknown_t p1 = xmalloc(size);
    if (p0 && p1) {
      memcpy(p1, p0->data, MIN(p0->size, size));
      free(p0->data);
      p0->data = p1;
      p0->size = size;
    }
  }

  return p;
}

handle_t bopen(const char* name) {
  MALLOCA(char, inp, 1024);
  if (name && '-' == name[0] && 0 == name[1]) {
    pbuffer_t p = bmalloc();
    if (p) {
      size_t size = fread(inp, 1, sizeof(inp), stdin);
      while (size) {
        p = bappend(p, inp, size);
        size = xread(inp, 1, sizeof(inp), stdin);
      }

      return p;
    }
  } else {
    FILE* f = fopen(name, "rb");
    if (f) {
      pbuffer_t p = bmalloc();
      if (p) {
        strname(p->note, name);
        p->size = fsize(f);
        p->data = xmalloc(p->size);
        if (p->size != fread(p->data, 1, p->size, f)) {
          p = xfree(p);
        }

        fclose(f);
      }

      return p;
    }
  }

  return NULL;
}

bool_t issafe(pbuffer_t p) {
  return p && p->data ? TRUE : FALSE;
}

bool_t isbuffer(handle_t p) {
  return ismode(p, MODE_BUFFER) && issafe(p);
}

unknown_t getp(const pbuffer_t p, const int offset, const size_t size) {
  if (issafe(p) && (offset + size - 1) < p->size) {
    return CAST(puchar_t, p->data) + offset;
  } else if(issafe(p)) {
//    printf_e("datasize = %d, offset = %d, size = %d\n", p->size, offset, size);
  }

  return NULL;
}

int getb(const pbuffer_t p, const int offset) {
  puchar_t v = getp(p, offset, sizeof(uchar_t));
  return v ? *v : -1;
}

int isBigEndian(const pbuffer_t p) {
  if (issafe(p)) {
    return ELFDATA2MSB == getb(p, EI_DATA);
  }

  return -1;
}

int isLittleEndian(const pbuffer_t p) {
  if (issafe(p)) {
    return ELFDATA2LSB == getb(p, EI_DATA);
  }

  return -1;
}

