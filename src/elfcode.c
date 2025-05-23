#include "printf.h"
#include "elfcode.h"
#include "memfind.h"

/* .tbss is special. It doesn't contribute memory space to normal
   segments and it doesn't take file space in normal segments. */
static bool_t check_tbss(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return ((sh_flags & SHF_TLS) != 0)
      && ((sh_type == SHT_NOBITS) && (p_type != PT_TLS));
}

static size_t get_size(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t sh_size, const uint64_t p_type) {
  return check_tbss(sh_flags, sh_type, p_type) ? 0 : sh_size;
}

/* Only PT_LOAD, PT_GNU_RELRO and PT_TLS segments can contain SHF_TLS sections. */
static bool_t check_x0a(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return ((sh_flags & SHF_TLS) != 0) &&
         ((p_type == PT_TLS) || (p_type == PT_GNU_RELRO) || (p_type == PT_LOAD));
}

/* PT_TLS segment contains only SHF_TLS sections, PT_PHDR no sections at all. */
static bool_t check_x0b(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return ((sh_flags & SHF_TLS) == 0) &&
         (p_type != PT_TLS) &&
         (p_type != PT_PHDR);
}

/* PT_LOAD and similar segments only have SHF_ALLOC sections. */
static bool_t check_x1a(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return (sh_flags & SHF_ALLOC) == 0;
}

static bool_t check_x1b(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return (p_type == PT_LOAD) ||
         (p_type == PT_DYNAMIC) ||
         (p_type == PT_GNU_EH_FRAME) ||
         (p_type == PT_GNU_STACK) ||
         (p_type == PT_GNU_RELRO) ||
         (p_type == PT_GNU_SFRAME) ||
         ((p_type >= PT_GNU_MBIND_LO) && (p_type <= PT_GNU_MBIND_HI));
}

/* Any section besides one of type SHT_NOBITS must have file offsets within the segment. */
static bool_t check_x2a(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return sh_type == SHT_NOBITS;
}

static bool_t check_x2b(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t sh_addr, const uint64_t sh_offset, const uint64_t sh_size,
                     const uint64_t p_type, const uint64_t p_vaddr, const uint64_t p_offset, const uint64_t p_memsz, const uint64_t p_filesz) {
  return (sh_offset >= p_offset) &&
         (sh_offset - p_offset <= p_filesz - 1) &&
         ((sh_offset - p_offset + get_size(sh_flags, sh_type, sh_size, p_type)) <= p_filesz);
}

/* SHF_ALLOC sections must have VMAs within the segment. */
static bool_t check_x3a(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return (sh_flags & SHF_ALLOC) == 0;
}

static bool_t check_x3b(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t sh_addr, const uint64_t sh_offset, const uint64_t sh_size,
                     const uint64_t p_type, const uint64_t p_vaddr, const uint64_t p_offset, const uint64_t p_memsz, const uint64_t p_filesz) {
  return (sh_addr >= p_vaddr) &&
         ((sh_addr - p_vaddr) <= (p_memsz - 1)) &&
         ((sh_addr - p_vaddr + get_size(sh_flags, sh_type, sh_size, p_type)) <= p_memsz);
}

/* No zero size sections at start or end of PT_DYNAMIC nor PT_NOTE. */
static bool_t check_x4a(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return p_type != PT_DYNAMIC && p_type != PT_NOTE;
}

static bool_t check_x4b(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t sh_addr, const uint64_t sh_offset, const uint64_t sh_size,
                     const uint64_t p_type, const uint64_t p_vaddr, const uint64_t p_offset, const uint64_t p_memsz, const uint64_t p_filesz) {
  return sh_size != 0 || p_memsz == 0;
}

static bool_t check_x4c(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t sh_addr, const uint64_t sh_offset, const uint64_t sh_size,
                     const uint64_t p_type, const uint64_t p_vaddr, const uint64_t p_offset, const uint64_t p_memsz, const uint64_t p_filesz) {
  return (sh_type == SHT_NOBITS || (sh_offset > p_offset && (sh_offset - p_offset < p_filesz))) &&
         ((sh_flags & SHF_ALLOC) == 0 || (sh_addr > p_vaddr && (sh_addr - p_vaddr < p_memsz)));
}

bool_t isTBSS32(Elf64_Shdr *s, Elf64_Phdr *p) {
  return s && p && check_tbss(s->sh_flags, s->sh_type, p->p_type);
}

bool_t isTBSS64(Elf64_Shdr *s, Elf64_Phdr *p) {
  return s && p && check_tbss(s->sh_flags, s->sh_type, p->p_type);
}

bool_t isshdrinphdr32(Elf32_Shdr *s, Elf32_Phdr *p) {
  if (s && p) {
    /* Only PT_LOAD, PT_GNU_RELRO and PT_TLS segments can contain SHF_TLS sections. */
    int x0a = check_x0a(s->sh_flags, s->sh_type, p->p_type);

    /* PT_TLS segment contains only SHF_TLS sections, PT_PHDR no sections at all. */
    int x0b = check_x0b(s->sh_flags, s->sh_type, p->p_type);

    /* PT_LOAD and similar segments only have SHF_ALLOC sections. */
    int x1a = check_x1a(s->sh_flags, s->sh_type, p->p_type);
    int x1b = check_x1b(s->sh_flags, s->sh_type, p->p_type);

    /* Any section besides one of type SHT_NOBITS must have file offsets within the segment. */
    int x2a = check_x2a(s->sh_flags, s->sh_type, p->p_type);
    int x2b = check_x2b(s->sh_flags, s->sh_type, s->sh_addr, s->sh_offset, s->sh_size,
                        p->p_type, p->p_vaddr, p->p_offset, p->p_memsz, p->p_filesz);

    /* SHF_ALLOC sections must have VMAs within the segment. */
    int x3a = check_x3a(s->sh_flags, s->sh_type, p->p_type);
    int x3b = check_x3b(s->sh_flags, s->sh_type, s->sh_addr, s->sh_offset, s->sh_size,
                        p->p_type, p->p_vaddr, p->p_offset, p->p_memsz, p->p_filesz);

    /* No zero size sections at start or end of PT_DYNAMIC nor PT_NOTE. */
    int x4a = check_x4a(s->sh_flags, s->sh_type, p->p_type);
    int x4b = check_x4b(s->sh_flags, s->sh_type, s->sh_addr, s->sh_offset, s->sh_size,
                        p->p_type, p->p_vaddr, p->p_offset, p->p_memsz, p->p_filesz);
    int x4c = check_x4c(s->sh_flags, s->sh_type, s->sh_addr, s->sh_offset, s->sh_size,
                        p->p_type, p->p_vaddr, p->p_offset, p->p_memsz, p->p_filesz);

    return (x0a || x0b) &&
          !(x1a && x1b) &&
           (x2a || x2b) &&
           (x3a || x3b) &&
           (x4a || x4b || x4c);
  }

  return FALSE;
}

bool_t isshdrinphdr64(Elf64_Shdr *s, Elf64_Phdr *p) {
  if (s && p) {
    /* Only PT_LOAD, PT_GNU_RELRO and PT_TLS segments can contain SHF_TLS sections. */
    int x0a = check_x0a(s->sh_flags, s->sh_type, p->p_type);

    /* PT_TLS segment contains only SHF_TLS sections, PT_PHDR no sections at all. */
    int x0b = check_x0b(s->sh_flags, s->sh_type, p->p_type);

    /* PT_LOAD and similar segments only have SHF_ALLOC sections. */
    int x1a = check_x1a(s->sh_flags, s->sh_type, p->p_type);
    int x1b = check_x1b(s->sh_flags, s->sh_type, p->p_type);

    /* Any section besides one of type SHT_NOBITS must have file offsets within the segment. */
    int x2a = check_x2a(s->sh_flags, s->sh_type, p->p_type);
    int x2b = check_x2b(s->sh_flags, s->sh_type, s->sh_addr, s->sh_offset, s->sh_size,
                        p->p_type, p->p_vaddr, p->p_offset, p->p_memsz, p->p_filesz);

    /* SHF_ALLOC sections must have VMAs within the segment. */
    int x3a = check_x3a(s->sh_flags, s->sh_type, p->p_type);
    int x3b = check_x3b(s->sh_flags, s->sh_type, s->sh_addr, s->sh_offset, s->sh_size,
                        p->p_type, p->p_vaddr, p->p_offset, p->p_memsz, p->p_filesz);

    /* No zero size sections at start or end of PT_DYNAMIC nor PT_NOTE. */
    int x4a = check_x4a(s->sh_flags, s->sh_type, p->p_type);
    int x4b = check_x4b(s->sh_flags, s->sh_type, s->sh_addr, s->sh_offset, s->sh_size,
                        p->p_type, p->p_vaddr, p->p_offset, p->p_memsz, p->p_filesz);
    int x4c = check_x4c(s->sh_flags, s->sh_type, s->sh_addr, s->sh_offset, s->sh_size,
                        p->p_type, p->p_vaddr, p->p_offset, p->p_memsz, p->p_filesz);

    return (x0a || x0b) &&
          !(x1a && x1b) &&
           (x2a || x2b) &&
           (x3a || x3b) &&
           (x4a || x4b || x4c);
  }

  return FALSE;
}

uint64_t getLE(const void *p, const size_t siz) {
  uint64_t x = 0;

  if (1 <= siz && siz <= 8) {
    for (size_t i = 0; i < siz; ++i) {
      x <<= 8;
      x |= ((uint64_t) ((unsigned char*)p)[siz - i - 1]);
    }
  } else {
    printf_x("unhandled data length: %ld", siz);
  }

  return x;
}

uint64_t getBE(const void *p, const size_t siz) {
  uint64_t x = 0;

  if (1 <= siz && siz <= 8) {
    for (size_t i = 0; i < siz; ++i) {
      x <<= 8;
      x |= ((uint64_t) ((unsigned char*)p)[i]);
    }
  } else {
    printf_x("unhandled data length: %ld", siz);
  }

  return x;
}

uint64_t get_eflags(const pbuffer_t p) {
  if (isELF32(p)) {
    Elf32_Ehdr *e = get_ehdr32(p);
    return e ? e->e_flags : 0;
  } else if (isELF64(p)) {
    Elf64_Ehdr *e = get_ehdr64(p);
    return e ? e->e_flags : 0;
  }

  return 0;
}

uint64_t get_etype(const pbuffer_t p) {
  if (isELF32(p)) {
    Elf32_Ehdr *e = get_ehdr32(p);
    return e ? e->e_type : 0;
  } else if (isELF64(p)) {
    Elf64_Ehdr *e = get_ehdr64(p);
    return e ? e->e_type : 0;
  }

  return 0;
}

uint64_t get_emachine(const pbuffer_t p) {
  if (isELF32(p)) {
    Elf32_Ehdr *e = get_ehdr32(p);
    return e ? e->e_machine : 0;
  } else if (isELF64(p)) {
    Elf64_Ehdr *e = get_ehdr64(p);
    return e ? e->e_machine : 0;
  }

  return 0;
}

bool_t isELF(const pbuffer_t p) {
  if (issafe(p)) {
    return 0x7f == getb(p, EI_MAG0) && 'E' == getb(p, EI_MAG1) && 'L' == getb(p, EI_MAG2) && 'F' == getb(p, EI_MAG3) ? TRUE : FALSE;
  }

  return FALSE;
}

bool_t isELF32(const pbuffer_t p) {
  if (isELF(p)) {
    return 1 == getb(p, EI_CLASS) ? TRUE : FALSE;
  }

  return FALSE;
}

bool_t isELF64(const pbuffer_t p) {
  if (isELF(p)) {
    return 2 == getb(p, EI_CLASS) ? TRUE : FALSE;
  }

  return FALSE;
}

Elf32_Ehdr* get_ehdr32(const pbuffer_t p) {
  return CAST(Elf32_Ehdr*, getp(p, 0, sizeof(Elf32_Ehdr)));
}

Elf64_Ehdr* get_ehdr64(const pbuffer_t p) {
  return CAST(Elf64_Ehdr*, getp(p, 0, sizeof(Elf64_Ehdr)));
}

Elf32_Nhdr* get_nhdr32byindex(const pbuffer_t p, const int index) {
  Elf32_Ehdr *e = get_ehdr32(p);
  if (e) {
    Elf32_Shdr *s = get_shdr32byindex(p, index);
    if (s && SHT_NOTE == s->sh_type) {
      return CAST(Elf32_Nhdr*, getp(p, s->sh_offset, s->sh_size));
    }
  }

  return NULL;
}

Elf64_Nhdr* get_nhdr64byindex(const pbuffer_t p, const int index) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    Elf64_Shdr *s = get_shdr64byindex(p, index);
    if (s && SHT_NOTE == s->sh_type) {
      return CAST(Elf64_Nhdr*, getp(p, s->sh_offset, s->sh_size));
    }
  }

  return NULL;
}

Elf32_Phdr* get_phdr32byindex(const pbuffer_t p, const int index) {
  Elf32_Ehdr *e = get_ehdr32(p);
  if (e) {
    return CAST(Elf32_Phdr*, getp(p, e->e_phoff + (e->e_phentsize * index), e->e_phentsize));
  }

  return NULL;
}

Elf64_Phdr* get_phdr64byindex(const pbuffer_t p, const int index) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    return CAST(Elf64_Phdr*, getp(p, e->e_phoff + (e->e_phentsize * index), e->e_phentsize));
  }

  return NULL;
}

Elf32_Shdr* get_shdr32bytype(const pbuffer_t p, const int type) {
  Elf32_Ehdr *e = get_ehdr32(p);
  if (e) {
    for (Elf32_Half i = 0; i < e->e_shnum; ++i) {
      Elf32_Shdr *s = get_shdr32byindex(p, i);
      if (s && s->sh_type == type){
        return s;
      }
    }
  }

  return NULL;
}

Elf64_Shdr* get_shdr64bytype(const pbuffer_t p, const int type) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
      Elf64_Shdr *s = get_shdr64byindex(p, i);
      if (s && s->sh_type == type){
        return s;
      }
    }
  }

  return NULL;
}

Elf32_Shdr* get_shdr32byname(const pbuffer_t p, const char* name) {
  Elf32_Ehdr *e = get_ehdr32(p);
  if (e && name && name[0]) {
    for (Elf32_Half i = 0; i < e->e_shnum; ++i) {
      const char* name0 = _get_secname32byindex(p, i);
      if (name0 && 0 == strcmp(name, name0)) {
        return get_shdr32byindex(p, i);
      }
    }
  }

  return NULL;
}

Elf64_Shdr* get_shdr64byname(const pbuffer_t p, const char* name) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e && name && name[0]) {
    for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
      const char* name0 = _get_secname64byindex(p, i);
      if (name0 && 0 == strcmp(name, name0)) {
        return get_shdr64byindex(p, i);
      }
    }
  }

  return NULL;
}

Elf32_Shdr* get_shdr32byindex(const pbuffer_t p, const int index) {
  Elf32_Ehdr *e = get_ehdr32(p);
  if (e) {
    return CAST(Elf32_Shdr*, getp(p, e->e_shoff + (e->e_shentsize * index), e->e_shentsize));
  }

  return NULL;
}

Elf64_Shdr* get_shdr64byindex(const pbuffer_t p, const int index) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    return CAST(Elf64_Shdr*, getp(p, e->e_shoff + (e->e_shentsize * index), e->e_shentsize));
  }

  return NULL;
}

size_t _get_secname32maxsize(const pbuffer_t p) {
  char *s0 = NULL;
  size_t siz = 0;

  Elf32_Ehdr *e = get_ehdr32(p);
  if (e) {
    Elf32_Shdr *s1 = get_shdr32byindex(p, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      for (Elf32_Half i = 0; i < e->e_shnum; ++i) {
        Elf32_Shdr *s2 = get_shdr32byindex(p, i);
        if (s2) {
          siz = MAX(siz, strlen(s0 + s2->sh_name));
        }
      }
    }
  }

  return siz;
}

size_t _get_secname64maxsize(const pbuffer_t p) {
  char *s0 = NULL;
  size_t siz = 0;

  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    Elf64_Shdr *s1 = get_shdr64byindex(p, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
        Elf64_Shdr *s2 = get_shdr64byindex(p, i);
        if (s2) {
          siz = MAX(siz, strlen(s0 + s2->sh_name));
        }
      }
    }
  }

  return siz;
}

size_t get_secnamemaxsize(const pbuffer_t p) {
  if (isELF(p)) {
    if (isELF32(p))        return _get_secname32maxsize(p);
    else if (isELF64(p))   return _get_secname64maxsize(p);
  }

  return 0;
}

const char* _get_secname32byindex(const pbuffer_t p, const int index) {
  char *s0 = NULL;

  Elf32_Ehdr *e = get_ehdr32(p);
  if (e) {
    Elf32_Shdr *s1 = get_shdr32byindex(p, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      Elf32_Shdr *s2 = get_shdr32byindex(p, index);
      if (s2) {
        return s0 + s2->sh_name;
      }
    }
  }

  return NULL;
}

const char* _get_secname64byindex(const pbuffer_t p, const int index) {
  char *s0 = NULL;

  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    Elf64_Shdr *s1 = get_shdr64byindex(p, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      Elf64_Shdr *s2 = get_shdr64byindex(p, index);
      if (s2) {
        return s0 + s2->sh_name;
      }
    }
  }

  return NULL;
}

const char* get_secnamebyindex(const pbuffer_t p, const int index) {
  if (isELF(p)) {
    if (isELF32(p))        return _get_secname32byindex(p, index);
    else if (isELF64(p))   return _get_secname64byindex(p, index);
  }

  return NULL;
}

const char* _get_name32byoffset(const pbuffer_t p, const int index, const int offset) {
  Elf32_Ehdr *e = get_ehdr32(p);
  if (e) {
    Elf32_Shdr *s = get_shdr32byindex(p, index);
    if (s) {
      return getp(p, s->sh_offset + offset, s->sh_size - offset);
    }
  }

  return NULL;
}

const char* _get_name64byoffset(const pbuffer_t p, const int index, const int offset) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    Elf64_Shdr *s = get_shdr64byindex(p, index);
    if (s) {
      return getp(p, s->sh_offset + offset, s->sh_size - offset);
    }
  }

  return NULL;
}

const char* get_namebyoffset(const pbuffer_t p, const int index, const int offset) {
  if (isELF(p)) {
    if (isELF32(p))        return _get_name32byoffset(p, index, offset);
    else if (isELF64(p))   return _get_name64byoffset(p, index, offset);
  }

  return NULL;
}

const char* _get_nhdrname32byindex(const pbuffer_t p, const int index) {
  Elf32_Nhdr *n = get_nhdr32byindex(p, index);
  if (n && n->n_namesz) {
    Elf32_Shdr *s = get_shdr32byindex(p, index);
    if (s) {
      return getp(p, s->sh_offset + sizeof(Elf32_Nhdr), n->n_namesz);
    }
  }

  return NULL;
}

const char* _get_nhdrname64byindex(const pbuffer_t p, const int index) {
  Elf64_Nhdr *n = get_nhdr64byindex(p, index);
  if (n && n->n_namesz) {
    Elf64_Shdr *s = get_shdr64byindex(p, index);
    if (s) {
      return getp(p, s->sh_offset + sizeof(Elf64_Nhdr), n->n_namesz);
    }
  }

  return NULL;
}

const char* get_nhdrnamebyindex(const pbuffer_t p, const int index) {
  if (isELF(p)) {
    if (isELF32(p))        return _get_nhdrname32byindex(p, index);
    else if (isELF64(p))   return _get_nhdrname64byindex(p, index);
  }

  return NULL;
}

Elf32_Word* get_nhdrdesc32byindex(const pbuffer_t p, const int index) {
  Elf32_Nhdr *n = get_nhdr32byindex(p, index);
  if (n && n->n_namesz && n->n_descsz) {
    Elf32_Shdr *s = get_shdr32byindex(p, index);
    if (s) {
      return getp(p, s->sh_offset + sizeof(Elf32_Nhdr) + n->n_namesz, n->n_descsz);
    }
  }

  return NULL;
}

Elf64_Word* get_nhdrdesc64byindex(const pbuffer_t p, const int index) {
  Elf64_Nhdr *n = get_nhdr64byindex(p, index);
  if (n && n->n_namesz && n->n_descsz) {
    Elf64_Shdr *s = get_shdr64byindex(p, index);
    if (s) {
      return getp(p, s->sh_offset + sizeof(Elf64_Nhdr) + n->n_namesz, n->n_descsz);
    }
  }

  return NULL;
}

const char* get_secname32byshdr(const pbuffer_t p, Elf32_Shdr *s) {
  char *s0 = NULL;

  Elf32_Ehdr *e = get_ehdr32(p);
  if (e) {
    Elf32_Shdr *s1 = get_shdr32byindex(p, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0 && s) {
      return s0 + s->sh_name;
    }
  }

  return NULL;
}

const char* get_secname64byshdr(const pbuffer_t p, Elf64_Shdr *s) {
  char *s0 = NULL;

  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    Elf64_Shdr *s1 = get_shdr64byindex(p, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0 && s) {
      return s0 + s->sh_name;
    }
  }

  return NULL;
}

const char* get_secnamebyoffset(const pbuffer_t p, const int offset) {
  if (isELF(p)) {
    if (isELF32(p))        return _get_secname32byoffset(p, offset);
    else if (isELF64(p))   return _get_secname64byoffset(p, offset);
  }

  return NULL;
}

const char* _get_secname32byoffset(const pbuffer_t p, const int offset) {
  char *s0 = NULL;

  Elf32_Ehdr *e = get_ehdr32(p);
  if (e) {
    Elf32_Shdr *s1 = get_shdr32byindex(p, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      return s0 + offset;
    }
  }

  return NULL;
}

const char* _get_secname64byoffset(const pbuffer_t p, const int offset) {
  char *s0 = NULL;

  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    Elf64_Shdr *s1 = get_shdr64byindex(p, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      return s0 + offset;
    }
  }

  return NULL;
}

void* _get32byshdr(const pbuffer_t p, Elf32_Shdr *shdr) {
  if (shdr) {
    return getp(p, shdr->sh_offset, shdr->sh_size);
  }

  return NULL;
}

void* _get64byshdr(const pbuffer_t p, Elf64_Shdr *shdr) {
  if (shdr) {
    return getp(p, shdr->sh_offset, shdr->sh_size);
  }

  return NULL;
}

handle_t fget32byshdr(const pbuffer_t p, Elf32_Shdr *shdr) {
  if (shdr) {
    return fmalloc(getp(p, shdr->sh_offset, shdr->sh_size), shdr->sh_size, shdr->sh_entsize);
  }

  return NULL;
}

handle_t fget64byshdr(const pbuffer_t p, Elf64_Shdr *shdr) {
  if (shdr) {
    return fmalloc(getp(p, shdr->sh_offset, shdr->sh_size), shdr->sh_size, shdr->sh_entsize);
  }

  return NULL;
}

