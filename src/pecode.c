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

PIMAGE_DATA_DIRECTORY get_datadirbyentry(const pbuffer_t p, const int index) {
  if (index >= IMAGE_DIRECTORY_ENTRY_EXPORT && index <= IMAGE_DIRECTORY_ENTRY_RESERVED) {
    if (isPE32(p)) {
      PIMAGE_NT_HEADERS32 p0 = get_nt32hdr(p);
      return p0 ? &p0->OptionalHeader.DataDirectory[index] : NULL;
    } else if (isPE64(p)) {
      PIMAGE_NT_HEADERS64 p0 = get_nt64hdr(p);
      return p0 ? &p0->OptionalHeader.DataDirectory[index] : NULL;
    }
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

PIMAGE_SECTION_HEADER get_sectionhdrbyentry(const pbuffer_t p, const int index) {
  if (index >= IMAGE_DIRECTORY_ENTRY_EXPORT && index <= IMAGE_DIRECTORY_ENTRY_RESERVED) {
    if (isPE32(p)) {
      PIMAGE_NT_HEADERS32 p0 = get_nt32hdr(p);
      PIMAGE_DATA_DIRECTORY p1 = get_datadirbyentry(p, index);
      if (p0 && p1) {
        for (int i = 0; i < p0->FileHeader.NumberOfSections; ++i) {
          PIMAGE_SECTION_HEADER p2 = get_sectionhdrbyindex(p, i);
          if (p2 && p2->VirtualAddress <= p1->VirtualAddress && p2->VirtualAddress + p2->SizeOfRawData > p1->VirtualAddress) {
            return p2;
          }
        }
      }
    } else if (isPE64(p)) {
      PIMAGE_NT_HEADERS64 p0 = get_nt64hdr(p);
      PIMAGE_DATA_DIRECTORY p1 = get_datadirbyentry(p, index);
      if (p0 && p1) {
        for (int i = 0; i < p0->FileHeader.NumberOfSections; ++i) {
          PIMAGE_SECTION_HEADER p2 = get_sectionhdrbyindex(p, i);
          if (p2 && p2->VirtualAddress <= p1->VirtualAddress && p2->VirtualAddress + p2->SizeOfRawData > p1->VirtualAddress) {
            return p2;
          }
        }
      }
    }
  }

  return NULL;
}

PIMAGE_SECTION_HEADER get_sectionhdrbyRVA(const pbuffer_t p, const uint64_t vaddr) {
  if (isPE32(p)) {
    PIMAGE_NT_HEADERS32 p0 = get_nt32hdr(p);
    if (p0) {
      for (int i = 0; i < p0->FileHeader.NumberOfSections; ++i) {
        PIMAGE_SECTION_HEADER p2 = get_sectionhdrbyindex(p, i);
        if (p2 && p2->VirtualAddress <= vaddr && p2->VirtualAddress + p2->SizeOfRawData > vaddr) {
          return p2;
        }
      }
    }
  } else if (isPE64(p)) {
    PIMAGE_NT_HEADERS64 p0 = get_nt64hdr(p);
    if (p0) {
      for (int i = 0; i < p0->FileHeader.NumberOfSections; ++i) {
        PIMAGE_SECTION_HEADER p2 = get_sectionhdrbyindex(p, i);
        if (p2 && p2->VirtualAddress <= vaddr && p2->VirtualAddress + p2->SizeOfRawData > vaddr) {
          return p2;
        }
      }
    }
  }

  return NULL;
}

PIMAGE_SECTION_HEADER get_sectionhdrbyname(const pbuffer_t p, const char* name) {
  if (isPE32(p)) {
    PIMAGE_NT_HEADERS32 p0 = get_nt32hdr(p);
    if (p0) {
      for (int i = 0; i < p0->FileHeader.NumberOfSections; ++i) {
        PIMAGE_SECTION_HEADER p1 = get_sectionhdrbyindex(p, i);
        if (p1 && 0 == strncmp(CAST(const char*, p1->Name), name, sizeof(p1->Name))) return p1;
      }
    }
  } else if (isPE64(p)) {
    PIMAGE_NT_HEADERS64 p0 = get_nt64hdr(p);
    if (p0) {
      for (int i = 0; i < p0->FileHeader.NumberOfSections; ++i) {
        PIMAGE_SECTION_HEADER p1 = get_sectionhdrbyindex(p, i);
        if (p1 && 0 == strncmp(CAST(const char*, p1->Name), name, sizeof(p1->Name))) return p1;
      }
    }
  }

  return NULL;
}

unknown_t get_chunkbyRVA(const pbuffer_t p, const int index, const uint64_t vaddr, const size_t size) {
  if (IMAGE_DIRECTORY_ENTRY_UNKNOWN == index) {
    PIMAGE_SECTION_HEADER p0 = get_sectionhdrbyRVA(p, vaddr);
    return p0 ? getp(p, peconvert2va(p0, vaddr), size) : NULL;
  } else {
    PIMAGE_SECTION_HEADER p0 = get_sectionhdrbyentry(p, index);
    return p0 ? getp(p, peconvert2va(p0, vaddr), size) : NULL;
  }

  return NULL;
}

unknown_t get_chunkbyentry(const pbuffer_t p, const int index) {
  PIMAGE_DATA_DIRECTORY p0 = get_datadirbyentry(p, index);
  if (p0) {
    if (IMAGE_DIRECTORY_ENTRY_IMPORT == index) {
      return get_chunkbyRVA(p, index, p0->VirtualAddress, sizeof(PIMAGE_IMPORT_DESCRIPTOR));
    } else if (IMAGE_DIRECTORY_ENTRY_EXPORT == index) {
      return get_chunkbyRVA(p, index, p0->VirtualAddress, sizeof(IMAGE_EXPORT_DIRECTORY));
    } else if (IMAGE_DIRECTORY_ENTRY_RESOURCE == index) {
      return get_chunkbyRVA(p, index, p0->VirtualAddress, sizeof(IMAGE_RESOURCE_DIRECTORY));
    } else if (IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG == index) {
      if (isPE32(p)) {
        return get_chunkbyRVA(p, index, p0->VirtualAddress, sizeof(IMAGE_LOAD_CONFIG_DIRECTORY32));
      } else if (isPE64(p)) {
        return get_chunkbyRVA(p, index, p0->VirtualAddress, sizeof(IMAGE_LOAD_CONFIG_DIRECTORY64));
      }
    } else if (IMAGE_DIRECTORY_ENTRY_DEBUG == index) {
      return get_chunkbyRVA(p, index, p0->VirtualAddress, sizeof(IMAGE_DEBUG_DIRECTORY));
    } else if (IMAGE_DIRECTORY_ENTRY_EXCEPTION == index) {
      return get_chunkbyRVA(p, index, p0->VirtualAddress, sizeof(IMAGE_RUNTIME_FUNCTION_ENTRY));
    }
  }

  return NULL;
}

unknown_t get_chunkbyname(const pbuffer_t p, const char* name) {
  PIMAGE_SECTION_HEADER p0 = get_sectionhdrbyname(p, name);
  return p0 ? getp(p, p0->PointerToRawData, p0->SizeOfRawData) : NULL;
}

DWORD get_dwordbyRVA(const pbuffer_t p, const int index, const uint64_t vaddr) {
  PDWORD p0 = get_chunkbyRVA(p, index, vaddr, sizeof(DWORD));
  return p0 ? *p0 : 0;
}

