#include <string.h>
#include <malloc.h>

#include "buffer.h"
#include "printf.h"
#include "elfcode.h"
#include "options.h"
#include "memlink.h"
#include "objutils.h"

static bool_t ismode0(unknown_t p, const int mode) {
  if (p) {
    const char* pc = p;
    if (MODE_GET0(mode) != pc[0])      return FALSE;
    else if (MODE_GET1(mode) != pc[1]) return FALSE;
    else if (MODE_GET2(mode) != pc[2]) return FALSE;
    return TRUE;
  }

  return FALSE;
}

handle_t setmode(handle_t p, const int mode) {
  if (p) {
    char* p0 = CAST(char*, p);
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
  if (ismode0(p, MODE_LINK)) {
    lfree(p);
  }

  return NULL;
}

unknown_t xfree(unknown_t p) {
  if (p) {
    if (ismode0(p, MODE_BUFFER)) {
      return bfree(p);
    } else if (ismode0(p, MODE_OPTIONS)) {
      return ofree(p);
    } else if (ismode0(p, MODE_ACTIONS)) {
      return afree(p);
    } else if (ismode0(p, MODE_LINK)) {
      xfree(CAST(pnode_t, p)->item);
      return lfree(p);
    }

    free(p);
  }

  return NULL;
}

handle_t bmalloc() {
  handle_t p = xmalloc(sizeof(buffer_t));
  return setmode(p, MODE_BUFFER);
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

handle_t bopen(const char* name) {
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

  return 0;
}

bool_t ismode(handle_t p, const int mode) {
  if (p) {
    const char* pc = p;
    if (MODE_GET0(mode) != pc[0]) return FALSE;
    if (MODE_GET1(mode) != pc[1]) return FALSE;
    if (MODE_GET2(mode) != pc[2]) return FALSE;
    if (MODE_GET3(mode) != pc[3]) return FALSE;
    return TRUE;
  }

  return FALSE;
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

int get(const pbuffer_t p, const int offset) {
  unsigned char *v = getp(p, offset, 1);
  if (v) {
    return *v;
  }

  return -1;
}

int isBigEndian(const pbuffer_t p) {
  if (issafe(p)) {
    return ELFDATA2MSB == get(p, EI_DATA);
  }

  return -1;
}

int isLittleEndian(const pbuffer_t p) {
  if (issafe(p)) {
    return ELFDATA2LSB == get(p, EI_DATA);
  }

  return -1;
}

