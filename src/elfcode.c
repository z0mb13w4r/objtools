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

