#include "printf.h"
#include "elfcode.h"

/* .tbss is special. It doesn't contribute memory space to normal
   segments and it doesn't take file space in normal segments. */
static int check_tbss(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return ((sh_flags & SHF_TLS) != 0)
      && ((sh_type == SHT_NOBITS) && (p_type != PT_TLS));
}

static size_t get_size(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t sh_size, const uint64_t p_type) {
  return check_tbss(sh_flags, sh_type, p_type) ? 0 : sh_size;
}

/* Only PT_LOAD, PT_GNU_RELRO and PT_TLS segments can contain SHF_TLS sections. */
static int check_x0a(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return ((sh_flags & SHF_TLS) != 0) &&
         ((p_type == PT_TLS) || (p_type == PT_GNU_RELRO) || (p_type == PT_LOAD));
}

/* PT_TLS segment contains only SHF_TLS sections, PT_PHDR no sections at all. */
static int check_x0b(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return ((sh_flags & SHF_TLS) == 0) &&
         (p_type != PT_TLS) &&
         (p_type != PT_PHDR);
}

/* PT_LOAD and similar segments only have SHF_ALLOC sections. */
static int check_x1a(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return (sh_flags & SHF_ALLOC) == 0;
}

static int check_x1b(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return (p_type == PT_LOAD) ||
         (p_type == PT_DYNAMIC) ||
         (p_type == PT_GNU_EH_FRAME) ||
         (p_type == PT_GNU_STACK) ||
         (p_type == PT_GNU_RELRO) ||
         (p_type == PT_GNU_SFRAME) ||
         ((p_type >= PT_GNU_MBIND_LO) && (p_type <= PT_GNU_MBIND_HI));
}

/* Any section besides one of type SHT_NOBITS must have file offsets within the segment. */
static int check_x2a(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return sh_type == SHT_NOBITS;
}

static int check_x2b(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t sh_addr, const uint64_t sh_offset, const uint64_t sh_size,
                     const uint64_t p_type, const uint64_t p_vaddr, const uint64_t p_offset, const uint64_t p_memsz, const uint64_t p_filesz) {
  return (sh_offset >= p_offset) &&
         (sh_offset - p_offset <= p_filesz - 1) &&
         ((sh_offset - p_offset + get_size(sh_flags, sh_type, sh_size, p_type)) <= p_filesz);
}

/* SHF_ALLOC sections must have VMAs within the segment. */
static int check_x3a(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return (sh_flags & SHF_ALLOC) == 0;
}

static int check_x3b(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t sh_addr, const uint64_t sh_offset, const uint64_t sh_size,
                     const uint64_t p_type, const uint64_t p_vaddr, const uint64_t p_offset, const uint64_t p_memsz, const uint64_t p_filesz) {
  return (sh_addr >= p_vaddr) &&
         ((sh_addr - p_vaddr) <= (p_memsz - 1)) &&
         ((sh_addr - p_vaddr + get_size(sh_flags, sh_type, sh_size, p_type)) <= p_memsz);
}

/* No zero size sections at start or end of PT_DYNAMIC nor PT_NOTE. */
static int check_x4a(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t p_type) {
  return p_type != PT_DYNAMIC && p_type != PT_NOTE;
}

static int check_x4b(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t sh_addr, const uint64_t sh_offset, const uint64_t sh_size,
                     const uint64_t p_type, const uint64_t p_vaddr, const uint64_t p_offset, const uint64_t p_memsz, const uint64_t p_filesz) {
  return sh_size != 0 || p_memsz == 0;
}

static int check_x4c(const uint64_t sh_flags, const uint64_t sh_type, const uint64_t sh_addr, const uint64_t sh_offset, const uint64_t sh_size,
                     const uint64_t p_type, const uint64_t p_vaddr, const uint64_t p_offset, const uint64_t p_memsz, const uint64_t p_filesz) {
  return (sh_type == SHT_NOBITS || (sh_offset > p_offset && (sh_offset - p_offset < p_filesz))) &&
         ((sh_flags & SHF_ALLOC) == 0 || (sh_addr > p_vaddr && (sh_addr - p_vaddr < p_memsz)));
}

int isTBSS32(Elf64_Shdr *s, Elf64_Phdr *p) {
  return s && p && check_tbss(s->sh_flags, s->sh_type, p->p_type);
}

int isTBSS64(Elf64_Shdr *s, Elf64_Phdr *p) {
  return s && p && check_tbss(s->sh_flags, s->sh_type, p->p_type);
}

int isshdrinphdr32(Elf32_Shdr *s, Elf32_Phdr *p) {
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

  return 0;
}

int isshdrinphdr64(Elf64_Shdr *s, Elf64_Phdr *p) {
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

  return 0;
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

int isELF(const pbuffer_t p) {
  if (issafe(p)) {
    return 0x7f == get(p, EI_MAG0) && 'E' == get(p, EI_MAG1) && 'L' == get(p, EI_MAG2) && 'F' == get(p, EI_MAG3) ? 1 : 0;
  }

  return 0;
}

int isELF32(const pbuffer_t p) {
  if (isELF(p)) {
    return 1 == get(p, EI_CLASS) ? 1 : 0;
  }

  return 0;
}

int isELF64(const pbuffer_t p) {
  if (isELF(p)) {
    return 2 == get(p, EI_CLASS) ? 1 : 0;
  }

  return 0;
}

Elf32_Ehdr* get_ehdr32(const pbuffer_t p) {
  return CAST(Elf32_Ehdr*, getp(p, 0, sizeof(Elf32_Ehdr)));
}

Elf64_Ehdr* get_ehdr64(const pbuffer_t p) {
  return CAST(Elf64_Ehdr*, getp(p, 0, sizeof(Elf64_Ehdr)));
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

size_t get_secname32maxsize(const pbuffer_t p) {
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

size_t get_secname64maxsize(const pbuffer_t p) {
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

const char* get_secname32byindex(const pbuffer_t p, const int index) {
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

const char* get_secname64byindex(const pbuffer_t p, const int index) {
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

const char* get_name32byoffset(const pbuffer_t p, const int index, const int offset) {
  Elf32_Ehdr *e = get_ehdr32(p);
  if (e) {
    Elf32_Shdr *s = get_shdr32byindex(p, index);
    if (s) {
      return getp(p, s->sh_offset + offset, s->sh_size - offset);
    }
  }

  return NULL;
}

const char* get_name64byoffset(const pbuffer_t p, const int index, const int offset) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    Elf64_Shdr *s = get_shdr64byindex(p, index);
    if (s) {
      return getp(p, s->sh_offset + offset, s->sh_size - offset);
    }
  }

  return NULL;
}

