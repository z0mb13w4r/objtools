#include "printf.h"
#include "elfcode.h"
#include "elfcode-endian.h"
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

bool_t isTBSS32(Elf32_Shdr *s, Elf32_Phdr *p) {
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

uint64_t ecget_eflags(const pbuffer_t p) {
  if (isELF32(p)) {
    MEMSTACK(Elf32_Ehdr, ex);
    Elf32_Ehdr *e = ecget_ehdr32(p, ex);
    return e ? e->e_flags : 0;
  } else if (isELF64(p)) {
    MEMSTACK(Elf64_Ehdr, ex);
    Elf64_Ehdr *e = ecget_ehdr64(p, ex);
    return e ? e->e_flags : 0;
  }

  return 0;
}

uint64_t ecget_etype(const pbuffer_t p) {
  if (isELF32(p)) {
    MEMSTACK(Elf32_Ehdr, ex);
    Elf32_Ehdr *e = ecget_ehdr32(p, ex);
    return e ? e->e_type : 0;
  } else if (isELF64(p)) {
    MEMSTACK(Elf64_Ehdr, ex);
    Elf64_Ehdr *e = ecget_ehdr64(p, ex);
    return e ? e->e_type : 0;
  }

  return 0;
}

uint64_t ecget_emachine(const pbuffer_t p) {
  if (isELF32(p)) {
    MEMSTACK(Elf32_Ehdr, ex);
    Elf32_Ehdr *e = ecget_ehdr32(p, ex);
    return e ? e->e_machine : 0;
  } else if (isELF64(p)) {
    MEMSTACK(Elf64_Ehdr, ex);
    Elf64_Ehdr *e = ecget_ehdr64(p, ex);
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
  return isELF(p) && 1 == getb(p, EI_CLASS) ? TRUE : FALSE;
}

bool_t isELF64(const pbuffer_t p) {
  return isELF(p) && 2 == getb(p, EI_CLASS) ? TRUE : FALSE;
}

bool_t isELFbe(const pbuffer_t p) {
  return isELF(p) && ELFDATA2MSB == getb(p, EI_DATA) ? TRUE : FALSE;
}

bool_t isELFle(const pbuffer_t p) {
  return isELF(p) && ELFDATA2LSB == getb(p, EI_DATA) ? TRUE : FALSE;
}

bool_t isELFpie32(const pbuffer_t p) {
  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e) {
    for (Elf32_Half i = 0; i < e->e_shnum; ++i) {
      MEMSTACK(Elf32_Shdr, sx);
      Elf32_Shdr *s = ecget_shdr32byindex(p, sx, i);
      if (s && SHT_DYNAMIC == s->sh_type) {
        size_t cnt = s->sh_size / s->sh_entsize;

        Elf32_Dyn *d = _get32byshdr(p, s);
        for (size_t j = 0; j < cnt; ++j, ++d) {
          if (DT_FLAGS_1 == d->d_tag && (DF_1_PIE & d->d_un.d_val)) {
            return TRUE;
          }
        }
      }
    }
  }

  return FALSE;
}

bool_t isELFpie64(const pbuffer_t p) {
  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e) {
    for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
      MEMSTACK(Elf64_Shdr, sx);
      Elf64_Shdr *s = ecget_shdr64byindex(p, sx, i);
      if (s && SHT_DYNAMIC == s->sh_type) {
        size_t cnt = s->sh_size / s->sh_entsize;

        Elf64_Dyn *d = _get64byshdr(p, s);
        for (size_t j = 0; j < cnt; ++j, ++d) {
          if (DT_FLAGS_1 == d->d_tag && (DF_1_PIE & d->d_un.d_val)) {
            return TRUE;
          }
        }
      }
    }
  }

  return FALSE;
}

bool_t isELFpie(const pbuffer_t p) {
  if (isELF(p)) {
    if (isELF32(p))        return isELFpie32(p);
    else if (isELF64(p))   return isELFpie64(p);
  }

  return FALSE;
}

Elf32_Dyn* ecget_dyn32byindex(const pbuffer_t p, const unknown_t q, const int index, const int entry) {
  MEMSTACK(Elf32_Shdr, sx);
  Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, index);
  if (s0 && SHT_DYNAMIC == s0->sh_type) {
    size_t cnt = s0->sh_size / s0->sh_entsize;
    if (entry < cnt) {
      return ecconvert_dyn32(p, q, getp(p, s0->sh_offset + (entry * s0->sh_entsize), s0->sh_entsize));
    }
  }

  return NULL;
}

Elf64_Dyn* ecget_dyn64byindex(const pbuffer_t p, const unknown_t q, const int index, const int entry) {
  MEMSTACK(Elf64_Shdr, sx);
  Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, index);
  if (s0 && SHT_DYNAMIC == s0->sh_type) {
    size_t cnt = s0->sh_size / s0->sh_entsize;
    if (entry < cnt) {
      return ecconvert_dyn64(p, q, getp(p, s0->sh_offset + (entry * s0->sh_entsize), s0->sh_entsize));
    }
  }

  return NULL;
}

unknown_t  ecget_dynbyindex(const pbuffer_t p, const int index, const int entry) {
  if (isELF(p)) {
    if (isELF32(p))        return ecget_dyn32byindex(p, xmalloc(sizeof(Elf32_Dyn), MODE_HEAP), index, entry);
    else if (isELF64(p))   return ecget_dyn64byindex(p, xmalloc(sizeof(Elf64_Dyn), MODE_HEAP), index, entry);
  }

  return NULL;
}

Elf32_Versym* ecget_vsym32byindex(const pbuffer_t p, const unknown_t q, const int index) {
  MEMSTACK(Elf32_Shdr, sx);
  Elf32_Shdr *s0 = ecget_shdr32bytype(p, sx, SHT_GNU_versym);
  if (s0) {
    size_t cnt = s0->sh_size / s0->sh_entsize;
    if (index < cnt) {
      MEMSTACK(Elf32_Versym, vx);
      return ecconvert_versym32(p, vx, getp(p, s0->sh_offset + (index * s0->sh_entsize), s0->sh_entsize));
    }
  }

  return NULL;
}

Elf64_Versym* ecget_vsym64byindex(const pbuffer_t p, const unknown_t q, const int index) {
  MEMSTACK(Elf64_Shdr, sx);
  Elf64_Shdr *s0 = ecget_shdr64bytype(p, sx, SHT_GNU_versym);
  if (s0) {
    size_t cnt = s0->sh_size / s0->sh_entsize;
    if (index < cnt) {
      MEMSTACK(Elf64_Versym, vx);
      return ecconvert_versym64(p, vx, getp(p, s0->sh_offset + (index * s0->sh_entsize), s0->sh_entsize));
    }
  }

  return NULL;
}

unknown_t ecget_vsymbyindex(const pbuffer_t p, const int index) {
  if (isELF(p)) {
    if (isELF32(p))        return ecget_vsym32byindex(p, xmalloc(sizeof(Elf32_Versym), MODE_HEAP), index);
    else if (isELF64(p))   return ecget_vsym64byindex(p, xmalloc(sizeof(Elf64_Versym), MODE_HEAP), index);
  }

  return NULL;

}

Elf32_Ehdr* ecget_ehdr32(const pbuffer_t p, const unknown_t q) {
  return ecconvert_ehdr32(p, q, getp(p, 0, sizeof(Elf32_Ehdr)));
}

Elf64_Ehdr* ecget_ehdr64(const pbuffer_t p, const unknown_t q) {
  return ecconvert_ehdr64(p, q, getp(p, 0, sizeof(Elf64_Ehdr)));
}

unknown_t ecget_ehdr(const pbuffer_t p) {
  if (isELF(p)) {
    if (isELF32(p))        return ecget_ehdr32(p, xmalloc(sizeof(Elf32_Ehdr), MODE_HEAP));
    else if (isELF64(p))   return ecget_ehdr64(p, xmalloc(sizeof(Elf64_Ehdr), MODE_HEAP));
  }

  return NULL;
}

Elf32_Nhdr* ecget_nhdr32byindex(const pbuffer_t p, const unknown_t q, const int index) {
  MEMSTACK(Elf32_Shdr, sx);
  Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, index);
  if (s0 && SHT_NOTE == s0->sh_type) {
    return ecconvert_nhdr32(p, q, getp(p, s0->sh_offset, s0->sh_size));
  }

  return NULL;
}

Elf64_Nhdr* ecget_nhdr64byindex(const pbuffer_t p, const unknown_t q, const int index) {
  MEMSTACK(Elf64_Shdr, sx);
  Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, index);
  if (s0 && SHT_NOTE == s0->sh_type) {
    return ecconvert_nhdr64(p, q, getp(p, s0->sh_offset, s0->sh_size));
  }

  return NULL;
}

unknown_t ecget_nhdrbyindex(const pbuffer_t p, const int index) {
  if (isELF(p)) {
    if (isELF32(p))        return ecget_nhdr32byindex(p, xmalloc(sizeof(Elf32_Nhdr), MODE_HEAP), index);
    else if (isELF64(p))   return ecget_nhdr64byindex(p, xmalloc(sizeof(Elf64_Nhdr), MODE_HEAP), index);
  }

  return NULL;
}

Elf32_Phdr* ecget_phdr32byindex(const pbuffer_t p, const unknown_t q, const int index) {
  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e) {
    return ecconvert_phdr32(p, q, getp(p, e->e_phoff + (e->e_phentsize * index), e->e_phentsize));
  }

  return NULL;
}

Elf64_Phdr* ecget_phdr64byindex(const pbuffer_t p, const unknown_t q, const int index) {
  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e) {
    return ecconvert_phdr64(p, q, getp(p, e->e_phoff + (e->e_phentsize * index), e->e_phentsize));
  }

  return NULL;
}

unknown_t ecget_phdrbyindex(const pbuffer_t p, const int index) {
  if (isELF(p)) {
    if (isELF32(p))        return ecget_phdr32byindex(p, xmalloc(sizeof(Elf32_Phdr), MODE_HEAP), index);
    else if (isELF64(p))   return ecget_phdr64byindex(p, xmalloc(sizeof(Elf64_Phdr), MODE_HEAP), index);
  }

  return NULL;
}

Elf32_Shdr* ecget_shdr32byoffset(const pbuffer_t p, const unknown_t q, const int offset) {
  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e) {
    for (Elf32_Half i = 0; i < e->e_shnum; ++i) {
      Elf32_Shdr *s = ecget_shdr32byindex(p, q, i);
      if (s && s->sh_offset == offset){
        return s;
      }
    }
  }

  return NULL;
}

Elf64_Shdr* ecget_shdr64byoffset(const pbuffer_t p, const unknown_t q, const int offset) {
  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e) {
    for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
      Elf64_Shdr *s = ecget_shdr64byindex(p, q, i);
      if (s && s->sh_offset == offset){
        return s;
      }
    }
  }

  return NULL;
}

unknown_t   ecget_shdrbyoffset(const pbuffer_t p, const int offset) {
  if (isELF(p)) {
    if (isELF32(p))        return ecget_shdr32byoffset(p, xmalloc(sizeof(Elf32_Shdr), MODE_HEAP), offset);
    else if (isELF64(p))   return ecget_shdr64byoffset(p, xmalloc(sizeof(Elf64_Shdr), MODE_HEAP), offset);
  }

  return NULL;
}

Elf32_Shdr* ecget_shdr32bytype(const pbuffer_t p, const unknown_t q, const int type) {
  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e) {
    for (Elf32_Half i = 0; i < e->e_shnum; ++i) {
      Elf32_Shdr *s = ecget_shdr32byindex(p, q, i);
      if (s && s->sh_type == type){
        return s;
      }
    }
  }

  return NULL;
}

Elf64_Shdr* ecget_shdr64bytype(const pbuffer_t p, const unknown_t q, const int type) {
  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e) {
    for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
      Elf64_Shdr *s = ecget_shdr64byindex(p, q, i);
      if (s && s->sh_type == type){
        return s;
      }
    }
  }

  return NULL;
}

unknown_t ecget_shdrbytype(const pbuffer_t p, const int type) {
  if (isELF(p)) {
    if (isELF32(p))        return ecget_shdr32bytype(p, xmalloc(sizeof(Elf32_Shdr), MODE_HEAP), type);
    else if (isELF64(p))   return ecget_shdr64bytype(p, xmalloc(sizeof(Elf64_Shdr), MODE_HEAP), type);
  }

  return NULL;
}

int _ecget_index32byname(const pbuffer_t p, const char* name) {
  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e && name && name[0]) {
    for (Elf32_Half i = 0; i < e->e_shnum; ++i) {
      const char* name0 = _ecget_secname32byindex(p, i);
      if (name0 && 0 == xstrcmp(name, name0)) {
        return i;
      }
    }
  }

  return -1;
}

int _ecget_index64byname(const pbuffer_t p, const char* name) {
  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e && name && name[0]) {
    for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
      const char* name0 = _ecget_secname64byindex(p, i);
      if (name0 && 0 == xstrcmp(name, name0)) {
        return i;
      }
    }
  }

  return -1;
}

int ecget_indexbyname(const pbuffer_t p, const char* name) {
  if (isELF(p)) {
    if (isELF32(p))        return _ecget_index32byname(p, name);
    else if (isELF64(p))   return _ecget_index64byname(p, name);
  }

  return -1;
}

Elf32_Shdr* ecget_shdr32byname(const pbuffer_t p, const unknown_t q, const char* name) {
  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e && name && name[0]) {
    for (Elf32_Half i = 0; i < e->e_shnum; ++i) {
      const char* name0 = _ecget_secname32byindex(p, i);
      if (name0 && 0 == xstrcmp(name, name0)) {
        return ecget_shdr32byindex(p, q, i);
      }
    }
  }

  return NULL;
}

Elf64_Shdr* ecget_shdr64byname(const pbuffer_t p, const unknown_t q, const char* name) {
  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e && name && name[0]) {
    for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
      const char* name0 = _ecget_secname64byindex(p, i);
      if (name0 && 0 == xstrcmp(name, name0)) {
        return ecget_shdr64byindex(p, q, i);
      }
    }
  }

  return NULL;
}

unknown_t   ecget_shdrbyname(const pbuffer_t p, const char* name) {
  if (isELF(p)) {
    if (isELF32(p))        return ecget_shdr32byname(p, xmalloc(sizeof(Elf32_Shdr), MODE_HEAP), name);
    else if (isELF64(p))   return ecget_shdr64byname(p, xmalloc(sizeof(Elf64_Shdr), MODE_HEAP), name);
  }

  return NULL;
}

Elf32_Shdr* ecget_shdr32byindex(const pbuffer_t p, const unknown_t q, const int index) {
  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e0 = ecget_ehdr32(p, ex);
  if (e0 && -1 != index) {
    return ecconvert_shdr32(p, q, getp(p, e0->e_shoff + (e0->e_shentsize * index), e0->e_shentsize));
  }

  return NULL;
}

Elf64_Shdr* ecget_shdr64byindex(const pbuffer_t p, const unknown_t q, const int index) {
  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e0 = ecget_ehdr64(p, ex);
  if (e0 && -1 != index) {
    return ecconvert_shdr64(p, q, getp(p, e0->e_shoff + (e0->e_shentsize * index), e0->e_shentsize));
  }

  return NULL;
}

unknown_t ecget_shdrbyindex(const pbuffer_t p, const int index) {
  if (isELF(p)) {
    if (isELF32(p))        return ecget_shdr32byindex(p, xmalloc(sizeof(Elf32_Shdr), MODE_HEAP), index);
    else if (isELF64(p))   return ecget_shdr64byindex(p, xmalloc(sizeof(Elf64_Shdr), MODE_HEAP), index);
  }

  return NULL;
}

Elf32_Sym* ecget_sym32byindex(const pbuffer_t p, const unknown_t q, const int index, const int entry) {
  MEMSTACK(Elf32_Shdr, sx);
  Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, index);
  if (s0 && (SHT_SYMTAB == s0->sh_type || SHT_DYNSYM == s0->sh_type)) {
    size_t cnt = s0->sh_size / s0->sh_entsize;
    if (entry < cnt) {
      return ecconvert_sym32(p, q, getp(p, s0->sh_offset + (entry * s0->sh_entsize), s0->sh_entsize));
    }
  }

  return NULL;
}

Elf64_Sym* ecget_sym64byindex(const pbuffer_t p, const unknown_t q, const int index, const int entry) {
  MEMSTACK(Elf64_Shdr, sx);
  Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, index);
  if (s0 && (SHT_SYMTAB == s0->sh_type || SHT_DYNSYM == s0->sh_type)) {
    size_t cnt = s0->sh_size / s0->sh_entsize;
    if (entry < cnt) {
      return ecconvert_sym64(p, q, getp(p, s0->sh_offset + (entry * s0->sh_entsize), s0->sh_entsize));
    }
  }

  return NULL;
}

unknown_t  ecget_symbyindex(const pbuffer_t p, const int index, const int entry) {
  if (isELF(p)) {
    if (isELF32(p))        return ecget_sym32byindex(p, xmalloc(sizeof(Elf32_Sym), MODE_HEAP), index, entry);
    else if (isELF64(p))   return ecget_sym64byindex(p, xmalloc(sizeof(Elf64_Sym), MODE_HEAP), index, entry);
  }

  return NULL;
}

size_t ecget_secnamemaxsize(const pbuffer_t p) {
  if (isELF(p)) {
    if (isELF32(p))        return _ecget_secname32maxsize(p);
    else if (isELF64(p))   return _ecget_secname64maxsize(p);
  }

  return 0;
}

size_t _ecget_secname32maxsize(const pbuffer_t p) {
  char *s0 = NULL;
  size_t siz = 0;

  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s1 = ecget_shdr32byindex(p, sx, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      for (Elf32_Half i = 0; i < e->e_shnum; ++i) {
        Elf32_Shdr *s2 = ecget_shdr32byindex(p, sx, i);
        if (s2) {
          siz = MAX(siz, xstrlen(s0 + s2->sh_name));
        }
      }
    }
  }

  return siz;
}

size_t _ecget_secname64maxsize(const pbuffer_t p) {
  char *s0 = NULL;
  size_t siz = 0;

  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s1 = ecget_shdr64byindex(p, sx, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
        Elf64_Shdr *s2 = ecget_shdr64byindex(p, sx, i);
        if (s2) {
          siz = MAX(siz, xstrlen(s0 + s2->sh_name));
        }
      }
    }
  }

  return siz;
}

const char* ecget_secnamebyindex(const pbuffer_t p, const int index) {
  if (isELF(p)) {
    if (isELF32(p))        return _ecget_secname32byindex(p, index);
    else if (isELF64(p))   return _ecget_secname64byindex(p, index);
  }

  return NULL;
}

const char* _ecget_secname32byindex(const pbuffer_t p, const int index) {
  char *s0 = NULL;

  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s1 = ecget_shdr32byindex(p, sx, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      Elf32_Shdr *s2 = ecget_shdr32byindex(p, sx, index);
      if (s2) {
        return s0 + s2->sh_name;
      }
    }
  }

  return NULL;
}

const char* _ecget_secname64byindex(const pbuffer_t p, const int index) {
  char *s0 = NULL;

  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s1 = ecget_shdr64byindex(p, sx, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      Elf64_Shdr *s2 = ecget_shdr64byindex(p, sx, index);
      if (s2) {
        return s0 + s2->sh_name;
      }
    }
  }

  return NULL;
}

const char* ecget_secnamebyoffset(const pbuffer_t p, const int offset) {
  if (isELF(p)) {
    if (isELF32(p))        return _ecget_secname32byoffset(p, offset);
    else if (isELF64(p))   return _ecget_secname64byoffset(p, offset);
  }

  return NULL;
}

const char* _ecget_secname32byoffset(const pbuffer_t p, const int offset) {
  char *s0 = NULL;

  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s1 = ecget_shdr32byindex(p, sx, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      return s0 + offset;
    }
  }

  return NULL;
}

const char* _ecget_secname64byoffset(const pbuffer_t p, const int offset) {
  char *s0 = NULL;

  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s1 = ecget_shdr64byindex(p, sx, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      return s0 + offset;
    }
  }

  return NULL;
}

const char* ecget_secnamebyaddr(const pbuffer_t p, const int addr) {
  if (isELF(p)) {
    if (isELF32(p))        return _ecget_secname32byaddr(p, addr);
    else if (isELF64(p))   return _ecget_secname64byaddr(p, addr);
  }

  return NULL;
}

const char* _ecget_secname32byaddr(const pbuffer_t p, const int vaddr) {
  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e) {
    for (Elf32_Half i = 0; i < e->e_shnum; ++i) {
      MEMSTACK(Elf32_Shdr, sx);
      Elf32_Shdr *s = ecget_shdr32byindex(p, sx, i);
      if (s && vaddr == s->sh_addr) {
        return ecget_secname32byshdr(p, s);
      }
    }
  }

  return NULL;
}

const char* _ecget_secname64byaddr(const pbuffer_t p, const int vaddr) {
  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e) {
    for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
      MEMSTACK(Elf64_Shdr, sx);
      Elf64_Shdr *s = ecget_shdr64byindex(p, sx, i);
      if (s && vaddr == s->sh_addr) {
        return ecget_secname64byshdr(p, s);
      }
    }
  }

  return NULL;
}

const char* ecget_namebyoffset(const pbuffer_t p, const int index, const int offset) {
  if (isELF(p)) {
    if (isELF32(p))        return _ecget_name32byoffset(p, index, offset);
    else if (isELF64(p))   return _ecget_name64byoffset(p, index, offset);
  }

  return NULL;
}

const char* _ecget_name32byoffset(const pbuffer_t p, const int index, const int offset) {
  MEMSTACK(Elf32_Shdr, sx);
  Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, index);
  if (s0) {
    return getp(p, s0->sh_offset + offset, s0->sh_size - offset);
  }

  return NULL;
}

const char* _ecget_name64byoffset(const pbuffer_t p, const int index, const int offset) {
  MEMSTACK(Elf64_Shdr, sx);
  Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, index);
  if (s0) {
    return getp(p, s0->sh_offset + offset, s0->sh_size - offset);
  }

  return NULL;
}

const char* ecget_namebyaddr(const pbuffer_t p, const int vaddr, uint64_t *offset) {
  if (isELF(p)) {
    if (isELF32(p))        return _ecget_name32byaddr(p, vaddr, offset);
    else if (isELF64(p))   return _ecget_name64byaddr(p, vaddr, offset);
  }

  return NULL;
}

static const char* _ecget_name32byaddr0(const pbuffer_t p, const int vaddr, uint64_t *offset) {
  MEMSTACK(Elf32_Shdr, sx);
  Elf32_Shdr *sh = ecget_shdr32bytype(p, sx, SHT_SYMTAB);
  if (sh) {
    size_t cnt = sh->sh_size / sh->sh_entsize;
    handle_t f = fgetbyshdr(p, sh);
    if (f) {
      const char *name = NULL;
      for (size_t j = 0; j < cnt; ++j) {
        Elf32_Sym *st = fget(f);
        if (st) {
          if (ELF_ST_TYPE(st->st_info) != STT_SECTION && ELF_ST_TYPE(st->st_info) != STT_NOTYPE) {
            if (offset) {
              if (st->st_value <= vaddr) {
                uint64_t offset0 = vaddr - st->st_value;
                if (offset0 < *offset) {
                  name = ecget_namebyoffset(p, sh->sh_link, st->st_name);
                  *offset = offset0;
                }
              }
            } else if (st->st_value == vaddr) {
//printf("+++%x+++", vaddr);
              name = ecget_namebyoffset(p, sh->sh_link, st->st_name);
              break;
            }
          } else if (STT_NOTYPE == ELF_ST_TYPE(st->st_info) && STB_GLOBAL == ELF_ST_BIND(st->st_info)) {
            if (st->st_value == vaddr) {
//printf("+++%x+++", vaddr);
              name = ecget_namebyoffset(p, sh->sh_link, st->st_name);
              break;
            }
          }

          f = fnext(f);
        }
      }

      return name && name[0] ? name : NULL;
    }
  }

  return NULL;
}

static const char* _ecget_name32byaddr1(const pbuffer_t p, const int vaddr, uint64_t *offset) {
  STATICA(char, name, 1024);
  MALLOCA(version_t, vnames, 1024);
  ecmake_versionnames32(p, vnames, NELEMENTS(vnames));

  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e) {
    for (Elf32_Half i = 0; i < e->e_shnum; ++i) {
      MEMSTACK(Elf32_Shdr, sx);
      Elf32_Shdr *sh = ecget_shdr32byindex(p, sx, i);
      if (sh) {
        if (SHT_RELA == sh->sh_type || SHT_REL == sh->sh_type || SHT_RELR == sh->sh_type) {
          size_t cnt = sh->sh_size / sh->sh_entsize;

          if (SHT_REL == sh->sh_type) {
//printf("++R++");
          } else if (SHT_RELA == sh->sh_type) {
            Elf32_Rela *r = _get32byshdr(p, sh);
            if (r) {
              for (size_t j = 0; j < cnt; ++j, ++r) {
                if (r->r_offset == vaddr) {
                  MEMSTACK(Elf32_Shdr, dx);
                  Elf32_Shdr *dh = ecget_shdr32byindex(p, dx, sh->sh_link);
                  if (dh) {
                    Elf32_Off  off = ELF32_R_SYM(r->r_info);
                    Elf32_Sym *sym = getp(p, dh->sh_offset + (off * dh->sh_entsize), dh->sh_entsize);
                    if (sym) {
                      const char* symname = ecget_namebyoffset(p, dh->sh_link, sym->st_name);
                      const char* secname = ecget_secnamebyindex(p, sym->st_shndx);

                      if (isused(get_RELTYPEDEF(p), ELF32_R_TYPE(r->r_info))) {
//printf("++RTD++");
                        if (symname && symname[0])         return symname;
                        else if (secname && secname[0])    return secname;
                      } else if (isused(get_RELTYPEVER(p), ELF32_R_TYPE(r->r_info))) {
//printf("++RTV++");
                        if (symname && symname[0]) {
                          const char *vername = NULL;
                          MEMSTACK(Elf32_Shdr, vx);
                          Elf32_Shdr *vh = ecget_shdr32bytype(p, vx, SHT_GNU_versym);
                          if (vh) {
                            Elf32_Versym *vs = getp(p, vh->sh_offset + (off * vh->sh_entsize), vh->sh_entsize);
                            if (vs) {
                              *vs = *vs & VERSYM_VERSION;
                              if (*vs && *vs < NELEMENTS(vnames)) {
                                vername = ecget_namebyoffset(p, vnames[0], vnames[*vs]);
                              }
                            }
                          }

                          int n = snprintf(name, NELEMENTS(name), "%s", symname);
                          if (vername && vername[0]) {
                            snprintf(name + n, NELEMENTS(name) - n, "@%s", vername);
                          }
//printf("++++");
                          return name;
                        }
                      }
                    }
                  }
                }
              }
            }
          } else if (SHT_RELR == sh->sh_type) {
//printf("++RR++");
          }
        }
      }
    }
  }

  return NULL;
}

static const char* _ecget_name32byaddr2(const pbuffer_t p, const int vaddr, uint64_t *offset) {
  return _ecget_secname32byaddr(p, vaddr);
}

const char* _ecget_name32byaddr(const pbuffer_t p, const int vaddr, uint64_t *offset) {
  const char* name = _ecget_name32byaddr0(p, vaddr, NULL);
  if (NULL == name) {
    name = _ecget_name32byaddr0(p, vaddr + 1, NULL);
  }
  if (NULL == name) {
    name = _ecget_name32byaddr1(p, vaddr, NULL);
  }
  if (NULL == name) {
    name = _ecget_name32byaddr2(p, vaddr, NULL);
  }
  if (NULL == name && offset) {
    uint64_t offset0 = UINT_MAX;
    name = _ecget_name32byaddr0(p, vaddr, &offset0);
    *offset = offset0;
  }

  return name;
}

static const char* _ecget_name64byaddr0(const pbuffer_t p, const int vaddr, uint64_t *offset) {
  MEMSTACK(Elf64_Shdr, sx);
  Elf64_Shdr *sh = ecget_shdr64bytype(p, sx, SHT_SYMTAB);
  if (sh) {
    size_t cnt = sh->sh_size / sh->sh_entsize;
    handle_t f = fgetbyshdr(p, sh);
    if (f) {
      const char *name = NULL;
      for (size_t j = 0; j < cnt; ++j) {
        Elf64_Sym *st = fget(f);
        if (st) {
          if (STT_SECTION != ELF_ST_TYPE(st->st_info) && STT_NOTYPE != ELF_ST_TYPE(st->st_info)) {
            if (offset) {
              if (st->st_value <= vaddr) {
                uint64_t offset0 = vaddr - st->st_value;
                if (offset0 < *offset) {
                  name = ecget_namebyoffset(p, sh->sh_link, st->st_name);
                  *offset = offset0;
                }
              }
            } else if (st->st_value == vaddr) {
//printf("+++%x+++", vaddr);
              name = ecget_namebyoffset(p, sh->sh_link, st->st_name);
              break;
            }
          } else if (STT_NOTYPE == ELF_ST_TYPE(st->st_info) && STB_GLOBAL == ELF_ST_BIND(st->st_info)) {
            if (st->st_value == vaddr) {
//printf("+++%x+++", vaddr);
              name = ecget_namebyoffset(p, sh->sh_link, st->st_name);
              break;
            }
          }

          f = fnext(f);
        }
      }

      return name && name[0] ? name : NULL;
    }
  }

  return NULL;
}

static const char* _ecget_name64byaddr1(const pbuffer_t p, const int vaddr, uint64_t *offset) {
  STATICA(char, name, 1024);
  MALLOCA(version_t, vnames, 1024);
  ecmake_versionnames64(p, vnames, NELEMENTS(vnames));

  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e) {
    for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
      MEMSTACK(Elf64_Shdr, sx);
      Elf64_Shdr *sh = ecget_shdr64byindex(p, sx, i);
      if (sh) {
        if (SHT_RELA == sh->sh_type || SHT_REL == sh->sh_type || SHT_RELR == sh->sh_type) {
          size_t cnt = sh->sh_size / sh->sh_entsize;

          if (SHT_REL == sh->sh_type) {
//printf("++R++");
          } else if (SHT_RELA == sh->sh_type) {
            Elf64_Rela *r = _get64byshdr(p, sh);
            if (r) {
              for (size_t j = 0; j < cnt; ++j, ++r) {
                if (r->r_offset == vaddr) {
                  MEMSTACK(Elf64_Shdr, dx);
                  Elf64_Shdr *dh = ecget_shdr64byindex(p, dx, sh->sh_link);
                  if (dh) {
                    Elf64_Off  off = ELF64_R_SYM(r->r_info);
                    Elf64_Sym *sym = getp(p, dh->sh_offset + (off * dh->sh_entsize), dh->sh_entsize);
                    if (sym) {
                      const char* symname = ecget_namebyoffset(p, dh->sh_link, sym->st_name);
                      const char* secname = ecget_secnamebyindex(p, sym->st_shndx);

                      if (isused(get_RELTYPEDEF(p), ELF64_R_TYPE(r->r_info))) {
//printf("++RTD++");
                        if (symname && symname[0])         return symname;
                        else if (secname && secname[0])    return secname;
                      } else if (isused(get_RELTYPEVER(p), ELF64_R_TYPE(r->r_info))) {
//printf("++RTV++");
                        if (symname && symname[0]) {
                          const char *vername = NULL;
                          MEMSTACK(Elf64_Shdr, vx);
                          Elf64_Shdr *vh = ecget_shdr64bytype(p, vx, SHT_GNU_versym);
                          if (vh) {
                            Elf64_Versym *vs = getp(p, vh->sh_offset + (off * vh->sh_entsize), vh->sh_entsize);
                            if (vs) {
                              *vs = *vs & VERSYM_VERSION;
                              if (*vs && *vs < NELEMENTS(vnames)) {
                                vername = ecget_namebyoffset(p, vnames[0], vnames[*vs]);
                              }
                            }
                          }

                          int n = snprintf(name, NELEMENTS(name), "%s", symname);
                          if (vername && vername[0]) {
                            snprintf(name + n, NELEMENTS(name) - n, "@%s", vername);
                          }
//printf("++++");
                          return name;
                        }
                      }
                    }
                  }
                }
              }
            }
          } else if (SHT_RELR == sh->sh_type) {
//printf("++RR++");
          }
        }
      }
    }
  }

  return NULL;
}

static const char* _ecget_name64byaddr2(const pbuffer_t p, const int vaddr, uint64_t *offset) {
  return _ecget_secname64byaddr(p, vaddr);
}

const char* _ecget_name64byaddr(const pbuffer_t p, const int vaddr, uint64_t *offset) {
  const char* name = _ecget_name64byaddr0(p, vaddr, NULL);
  if (NULL == name) {
    name = _ecget_name64byaddr0(p, vaddr + 1, NULL);
  }
  if (NULL == name) {
    name = _ecget_name64byaddr1(p, vaddr, NULL);
  }
  if (NULL == name) {
    name = _ecget_name64byaddr2(p, vaddr, NULL);
  }
  if (NULL == name && offset) {
    uint64_t offset0 = UINT_MAX;
    name = _ecget_name64byaddr0(p, vaddr, &offset0);
    *offset = offset0;
  }

  return name;
}

const char* ecget_nhdrnamebyindex(const pbuffer_t p, const int index) {
  if (isELF(p)) {
    if (isELF32(p))        return _ecget_nhdrname32byindex(p, index);
    else if (isELF64(p))   return _ecget_nhdrname64byindex(p, index);
  }

  return NULL;
}

const char* _ecget_nhdrname32byindex(const pbuffer_t p, const int index) {
  MEMSTACK(Elf32_Nhdr, nx);
  Elf32_Nhdr *n0 = ecget_nhdr32byindex(p, nx, index);
  if (n0 && n0->n_namesz) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, index);
    if (s0) {
      return getp(p, s0->sh_offset + sizeof(Elf32_Nhdr), n0->n_namesz);
    }
  }

  return NULL;
}

const char* _ecget_nhdrname64byindex(const pbuffer_t p, const int index) {
  MEMSTACK(Elf64_Nhdr, nx);
  Elf64_Nhdr *n0 = ecget_nhdr64byindex(p, nx, index);
  if (n0 && n0->n_namesz) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, index);
    if (s0) {
      return getp(p, s0->sh_offset + sizeof(Elf64_Nhdr), n0->n_namesz);
    }
  }

  return NULL;
}

const char* ecget_symnamebyindex(const pbuffer_t p, const int index) {
  if (isELF(p)) {
    if (isELF32(p))        return _ecget_symname32byindex(p, index);
    else if (isELF64(p))   return _ecget_symname64byindex(p, index);
  }

  return NULL;
}

const char* _ecget_symname32byindex(const pbuffer_t p, const int index) {
  MEMSTACK(Elf32_Shdr, sx);
  Elf32_Shdr *sh = ecget_shdr32bytype(p, sx, SHT_SYMTAB);
  if (sh) {
    handle_t f = fgetbyshdr(p, sh);
    if (f) {
      Elf32_Sym *st = fmove(f, index * sh->sh_entsize);
      if (st && ELF_ST_TYPE(st->st_info) != STT_SECTION) {
        return ecget_namebyoffset(p, sh->sh_link, st->st_name);
      }
    }
  }

  return NULL;
}

const char* _ecget_symname64byindex(const pbuffer_t p, const int index) {
  MEMSTACK(Elf64_Shdr, sx);
  Elf64_Shdr *sh = ecget_shdr64bytype(p, sx, SHT_SYMTAB);
  if (sh) {
    handle_t f = fgetbyshdr(p, sh);
    if (f) {
      Elf64_Sym *st = fmove(f, index * sh->sh_entsize);
      if (st && ELF_ST_TYPE(st->st_info) != STT_SECTION) {
        return ecget_namebyoffset(p, sh->sh_link, st->st_name);
      }
    }
  }

  return NULL;
}

Elf32_Word* ecget_nhdrdesc32byindex(const pbuffer_t p, const int index) {
  MEMSTACK(Elf32_Nhdr, nx);
  Elf32_Nhdr *n0 = ecget_nhdr32byindex(p, nx, index);
  if (n0 && n0->n_namesz && n0->n_descsz) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s0 = ecget_shdr32byindex(p, sx, index);
    if (s0) {
      return getp(p, s0->sh_offset + sizeof(Elf32_Nhdr) + n0->n_namesz, n0->n_descsz);
    }
  }

  return NULL;
}

Elf64_Word* ecget_nhdrdesc64byindex(const pbuffer_t p, const int index) {
  MEMSTACK(Elf64_Nhdr, nx);
  Elf64_Nhdr *n0 = ecget_nhdr64byindex(p, nx, index);
  if (n0 && n0->n_namesz && n0->n_descsz) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s0 = ecget_shdr64byindex(p, sx, index);
    if (s0) {
      return getp(p, s0->sh_offset + sizeof(Elf64_Nhdr) + n0->n_namesz, n0->n_descsz);
    }
  }

  return NULL;
}

const char* ecget_secname32byshdr(const pbuffer_t p, Elf32_Shdr *s) {
  char *s0 = NULL;

  MEMSTACK(Elf32_Ehdr, ex);
  Elf32_Ehdr *e = ecget_ehdr32(p, ex);
  if (e) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr *s1 = ecget_shdr32byindex(p, sx, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0 && s) {
      return s0 + s->sh_name;
    }
  }

  return NULL;
}

const char* ecget_secname64byshdr(const pbuffer_t p, Elf64_Shdr *s) {
  char *s0 = NULL;

  MEMSTACK(Elf64_Ehdr, ex);
  Elf64_Ehdr *e = ecget_ehdr64(p, ex);
  if (e) {
    MEMSTACK(Elf64_Shdr, sx);
    Elf64_Shdr *s1 = ecget_shdr64byindex(p, sx, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0 && s) {
      return s0 + s->sh_name;
    }
  }

  return NULL;
}

unknown_t ecget_rawdatabyname(const pbuffer_t p, const char* name) {
  if (isELF(p)) {
    if (isELF32(p))        return _ecget_rawdata32byname(p, name);
    else if (isELF64(p))   return _ecget_rawdata64byname(p, name);
  }

  return NULL;
}

unknown_t _ecget_rawdata32byname(const pbuffer_t p, const char* name) {
  MEMSTACK(Elf32_Shdr, sx);
  Elf32_Shdr* s0 = ecget_shdr32byname(p, sx, name);
  return s0 ? getp(p, s0->sh_offset, s0->sh_size) : NULL;
}

unknown_t _ecget_rawdata64byname(const pbuffer_t p, const char* name) {
  MEMSTACK(Elf64_Shdr, sx);
  Elf64_Shdr* s0 = ecget_shdr64byname(p, sx, name);
  return s0 ? getp(p, s0->sh_offset, s0->sh_size) : NULL;
}

unknown_t _get32byshdr(const pbuffer_t p, Elf32_Shdr *shdr) {
  if (shdr) {
    return getp(p, shdr->sh_offset, shdr->sh_size);
  }

  return NULL;
}

unknown_t _get64byshdr(const pbuffer_t p, Elf64_Shdr *shdr) {
  if (shdr) {
    return getp(p, shdr->sh_offset, shdr->sh_size);
  }

  return NULL;
}

handle_t fgetbyshdr(const pbuffer_t p, unknown_t shdr) {
  if (isELF(p)) {
    if (isELF32(p))        return fget32byshdr(p, shdr);
    else if (isELF64(p))   return fget64byshdr(p, shdr);
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

int ecmake_sectionthumbs(const pbuffer_t p, pthumb_t thumbs, const size_t maxthumbs) {
  if (EM_ARM == ecget_emachine(p)) {
    MEMSTACK(Elf32_Shdr, sx);
    Elf32_Shdr* shdr = ecget_shdr32byname(p, sx, ".symtab");
    if (shdr && SHT_SYMTAB == shdr->sh_type) {
      size_t cnt = shdr->sh_size / shdr->sh_entsize;

      handle_t f = fget32byshdr(p, shdr);
      if (f) {
        int n = 0;
        for (size_t j = 0; j < cnt; ++j) {
          Elf32_Sym *s = fget(f);
          if (s) {
            const char* name = ecget_namebyoffset(p, shdr->sh_link, s->st_name);
            if (name && '$' == name[0] && 0 == name[2]) {
              if (thumbs && n < maxthumbs) {
//printf("%x|%s\n", s->st_value, name);
                thumbs[n].vaddr = s->st_value;
                thumbs[n].value = name[1];
                ++n;
              } else if (0 == maxthumbs) {
                ++n;
              }
            }

            f = fnext(f);
          }
        }

        return n;
      }
    }
  }

  return 0;
}

int ecmake_versionnames32(const pbuffer_t p, pversion_t vnames, const size_t maxvnames) {
  MEMSTACK(Elf32_Shdr, vx);
  Elf32_Shdr *vh = ecget_shdr32bytype(p, vx, SHT_GNU_verneed);
  if (vh) {
    Elf32_Word offset0 = 0;
    vnames[0] = vh->sh_link;

    for (Elf32_Word j = 0; j < vh->sh_info; ++j) {
      MEMSTACK(Elf32_Verneed, vy);
      Elf32_Verneed *vn = ecconvert_verneed32(p, vy, getp(p, vh->sh_offset + offset0, sizeof(Elf32_Verneed)));
      if (vn) {
        Elf32_Word offset1 = offset0 + vn->vn_aux;
        for (Elf64_Half k = 0; k < vn->vn_cnt; ++k) {
          MEMSTACK(Elf32_Vernaux, vz);
          Elf32_Vernaux *va = ecconvert_vernaux32(p, vz, getp(p, vh->sh_offset + offset1, sizeof(Elf32_Vernaux)));
          if (va) {
            if (va->vna_other < maxvnames) {
              vnames[va->vna_other] = va->vna_name;
            }
            offset1 += va->vna_next;
          }
        }

        offset0 += vn->vn_next;
      }
    }

    return vh->sh_link;
  }

  return 0;
}

int ecmake_versionnames64(const pbuffer_t p, pversion_t vnames, const size_t maxvnames) {
  MEMSTACK(Elf64_Shdr, vx);
  Elf64_Shdr *vh = ecget_shdr64bytype(p, vx, SHT_GNU_verneed);
  if (vh) {
    Elf64_Word offset0 = 0;
    vnames[0] = vh->sh_link;

    for (Elf64_Word j = 0; j < vh->sh_info; ++j) {
      MEMSTACK(Elf64_Verneed, vy);
      Elf64_Verneed *vn = ecconvert_verneed64(p, vy, getp(p, vh->sh_offset + offset0, sizeof(Elf64_Verneed)));
      if (vn) {
        Elf64_Word offset1 = offset0 + vn->vn_aux;
        for (Elf64_Half k = 0; k < vn->vn_cnt; ++k) {
          MEMSTACK(Elf64_Vernaux, vz);
          Elf64_Vernaux *va = ecconvert_vernaux64(p, vz, getp(p, vh->sh_offset + offset1, sizeof(Elf64_Vernaux)));
          if (va) {
            if (va->vna_other < maxvnames) {
              vnames[va->vna_other] = va->vna_name;
            }
            offset1 += va->vna_next;
          }
        }

        offset0 += vn->vn_next;
      }
    }

    return vh->sh_link;
  }

  return 0;
}

