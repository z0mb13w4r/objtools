#include "pecode.h"

typedef struct machine_s {
  uchar_t x0;
  uchar_t x1;
} machine_t, *pmachine_t;

static machine_t zIS32BIT[] = {
  {0xc0, 0x01},
  {0x32, 0x50},
  {0x4c, 0x01},
  {0x00, 0x00}
};

static machine_t zIS64BIT[] = {
  {0x64, 0x86},
  {0x64, 0xaa},
  {0x64, 0x50},
  {0x00, 0x00}
};

static bool_t checkbytes(const pbuffer_t p, const pmachine_t m) {
  if (m && isPE(p)) {
    PIMAGE_DOS_HEADER p0 = get_doshdr(p);
    if (p0) {
      const uchar_t x0 = get(p, p0->e_lfanew + 4);
      const uchar_t x1 = get(p, p0->e_lfanew + 5);
      for (int i = 0; m[i].x0 && m[i].x1; ++i) {
        if (x0 == m[i].x0 && x1 == m[i].x1) return TRUE;
      }
    }
  }

  return FALSE;
}

bool_t isPE(const pbuffer_t p) {
  if (issafe(p)) {
    if ('M' == get(p, 0) && 'Z' == get(p, 1)) {
      PIMAGE_DOS_HEADER p0 = get_doshdr(p);
      return p0 && 'P' == get(p, p0->e_lfanew + 0)
                && 'E' == get(p, p0->e_lfanew + 1)
                && 0x0 == get(p, p0->e_lfanew + 2)
                && 0x0 == get(p, p0->e_lfanew + 3) ? TRUE : FALSE;
    }
  }

  return FALSE;
}

bool_t isPE32(const pbuffer_t p) {
  return isPE(p) && checkbytes(p, zIS32BIT);
}

bool_t isPE64(const pbuffer_t p) {
  return isPE(p) && checkbytes(p, zIS64BIT);
}

PIMAGE_DOS_HEADER get_doshdr(const pbuffer_t p) {
  return CAST(PIMAGE_DOS_HEADER, getp(p, 0, sizeof(IMAGE_DOS_HEADER)));
}

PIMAGE_NT_HEADERS32 get_nt32hdr(const pbuffer_t p) {
  if (isPE32(p)) {
    PIMAGE_DOS_HEADER p0 = get_doshdr(p);
    return p0 ? CAST(PIMAGE_NT_HEADERS32, getp(p, p0->e_lfanew, sizeof(IMAGE_NT_HEADERS32))) : NULL;
  }

  return NULL;
}

PIMAGE_NT_HEADERS64 get_nt64hdr(const pbuffer_t p) {
  if (isPE64(p)) {
    PIMAGE_DOS_HEADER p0 = get_doshdr(p);
    return p0 ? CAST(PIMAGE_NT_HEADERS64, getp(p, p0->e_lfanew, sizeof(IMAGE_NT_HEADERS64))) : NULL;
  }

  return NULL;
}

PIMAGE_SECTION_HEADER get_sectionhdrbyindex(const pbuffer_t p, const int index) {
  if (isPE32(p)) {
    PIMAGE_DOS_HEADER p0 = get_doshdr(p);
    PIMAGE_NT_HEADERS32 p1 = get_nt32hdr(p);
    if (p0 && p1) {
      const size_t spos = p0->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + p1->FileHeader.SizeOfOptionalHeader;
      const size_t size = sizeof(IMAGE_SECTION_HEADER);
      return CAST(PIMAGE_SECTION_HEADER, getp(p, spos + (index * size), size));
    }
  } else if (isPE64(p)) {
    PIMAGE_DOS_HEADER p0 = get_doshdr(p);
    PIMAGE_NT_HEADERS64 p1 = get_nt64hdr(p);
    if (p0 && p1) {
      const size_t spos = p0->e_lfanew + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + p1->FileHeader.SizeOfOptionalHeader;
      const size_t size = sizeof(IMAGE_SECTION_HEADER);
      return CAST(PIMAGE_SECTION_HEADER, getp(p, spos + (index * size), size));
    }
  }

  return NULL;
}

