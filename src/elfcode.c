#include "elfcode.h"

/* .tbss is special. It doesn't contribute memory space to normal
   segments and it doesn't take file space in normal segments. */
static int istTBSS(Elf64_Shdr *s, Elf64_Phdr *p) {
  return ((s->sh_flags & SHF_TLS) != 0)
      && ((s->sh_type == SHT_NOBITS) && (p->p_type != PT_TLS));
}

static size_t get_shdrsize(Elf64_Shdr *s, Elf64_Phdr *p) {
  return istTBSS(s, p) ? 0 : s->sh_size;
}

int shdrinphdr64(Elf64_Shdr *s, Elf64_Phdr *p) {
  /* Only PT_LOAD, PT_GNU_RELRO and PT_TLS segments can contain SHF_TLS sections. */
  int x0a = ((s->sh_flags & SHF_TLS) != 0) &&
            ((p->p_type == PT_TLS) || (p->p_type == PT_GNU_RELRO) || (p->p_type == PT_LOAD));
  /* PT_TLS segment contains only SHF_TLS sections, PT_PHDR no sections at all. */
  int x0b = ((s->sh_flags & SHF_TLS) == 0) &&
            (p->p_type != PT_TLS) &&
            (p->p_type != PT_PHDR);

  /* PT_LOAD and similar segments only have SHF_ALLOC sections. */
  int x1a = ((s->sh_flags & SHF_ALLOC) == 0);
  int x1b = (p->p_type == PT_LOAD) ||
            (p->p_type == PT_DYNAMIC) ||
            (p->p_type == PT_GNU_EH_FRAME) ||
            (p->p_type == PT_GNU_STACK) ||
            (p->p_type == PT_GNU_RELRO) ||
            (p->p_type == PT_GNU_SFRAME) ||
            ((p->p_type >= PT_GNU_MBIND_LO) && (p->p_type <= PT_GNU_MBIND_HI));

  /* Any section besides one of type SHT_NOBITS must have file offsets within the segment. */
  int x2a = (s->sh_type == SHT_NOBITS);
  int x2b = (s->sh_offset >= p->p_offset) &&
            (s->sh_offset - p->p_offset <= p->p_filesz - 1) &&
            ((s->sh_offset - p->p_offset + get_shdrsize(s, p)) <= p->p_filesz);
  /* SHF_ALLOC sections must have VMAs within the segment. */
  int x3a = ((s->sh_flags & SHF_ALLOC) == 0);
  int x3b = (s->sh_addr >= p->p_vaddr) &&
            ((s->sh_addr - p->p_vaddr) <= (p->p_memsz - 1)) &&
            ((s->sh_addr - p->p_vaddr + get_shdrsize(s, p)) <= p->p_memsz);

  /* No zero size sections at start or end of PT_DYNAMIC nor PT_NOTE.  */
  int x4a = (p->p_type != PT_DYNAMIC && p->p_type != PT_NOTE);
  int x4b = (s->sh_size != 0 || p->p_memsz == 0);
  int x4c = (s->sh_type == SHT_NOBITS || (s->sh_offset > p->p_offset && (s->sh_offset - p->p_offset < p->p_filesz))) &&
            ((s->sh_flags & SHF_ALLOC) == 0 || (s->sh_addr > p->p_vaddr && (s->sh_addr - p->p_vaddr < p->p_memsz)));

  return (x0a || x0b) &&
        !(x1a && x1b) &&
         (x2a || x2b) &&
         (x3a || x3b) &&
         (x4a || x4b || x4c);
}

uint64_t getLE(const void *p, const size_t siz) {
  uint64_t x = 0;

  if (1 <= siz && siz <= 8) {
    for (size_t i = 0; i < siz; ++i) {
      x <<= 8;
      x |= ((uint64_t) ((unsigned char*)p)[siz - i - 1]);
    }
  } else {
    printf("ERROR: unhandled data length: %ld\n", siz);
    abort ();
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
    printf("ERROR: unhandled data length: %ld\n", siz);
    abort ();
  }

  return x;
}

int isELF(const pbuffer_t p) {
  if (issafe(p)) {
    return 0x7f == get(p, EI_MAG0) && 'E' == get(p, EI_MAG1) && 'L' == get(p, EI_MAG2) && 'F' == get(p, EI_MAG3) ? 1 : 0;
  }

  return -1;
}

int is32(const pbuffer_t p) {
  if (issafe(p)) {
    return 1 == get(p, EI_CLASS) ? 1 : 0;
  }

  return -1;
}

int is64(const pbuffer_t p) {
  if (issafe(p)) {
    return 2 == get(p, EI_CLASS) ? 1 : 0;
  }

  return -1;
}

Elf32_Ehdr* get_ehdr32(const pbuffer_t p) {
  return CAST(Elf32_Ehdr*, getp(p, 0, sizeof(Elf32_Ehdr)));
}

Elf64_Ehdr* get_ehdr64(const pbuffer_t p) {
  return CAST(Elf64_Ehdr*, getp(p, 0, sizeof(Elf64_Ehdr)));
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

