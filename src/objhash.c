#include <string.h>

#include "printf.h"
#include "elfcode.h"
#include "objhash.h"

static int dump_create0(const pbuffer_t p, const poptions_t o, const char* name,
                        const uint64_t sh_type, const uint64_t sh_offset, const uint64_t sh_size, const uint64_t sh_addr,
                        const int maxsize) {
  int n = 0;
  n += printf_sore(getp(p, sh_offset, sh_size), sh_size, USE_SHA256 | USE_NOTEXT);
  n += printf_text(name, USE_LT | USE_SPACE | SET_PAD(maxsize));
  n += printf_pick(zSHDRTYPE, sh_type, USE_LT | USE_SPACE | SET_PAD(16));
  n += printf_nice(sh_addr, isELF64(p) ? USE_LHEX64 : USE_LHEX32);
  n += printf_nice(sh_offset, USE_LHEX32);
  n += printf_nice(sh_size, USE_LHEX32);
  n += printf_eol();

  return n;
}

static int dump_createELF32(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = MAX(get_secnamemaxsize(p) + 2, 21);

  int n = 0;
  Elf32_Ehdr *ehdr = get_ehdr32(p);
  if (ehdr) {
    for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
      Elf32_Shdr *shdr = get_shdr32byindex(p, i);
      if (shdr) {
        n += dump_create0(p, o, get_secnamebyindex(p, i), shdr->sh_type, shdr->sh_offset, shdr->sh_size, shdr->sh_addr, MAXSIZE);
      }
    }
  }

  return n;
}

static int dump_createELF64(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = MAX(get_secnamemaxsize(p) + 2, 21);

  int n = 0;
  Elf64_Ehdr *ehdr = get_ehdr64(p);
  if (ehdr) {
    for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
      Elf64_Shdr *shdr = get_shdr64byindex(p, i);
      if (shdr) {
        n += dump_create0(p, o, get_secnamebyindex(p, i), shdr->sh_type, shdr->sh_offset, shdr->sh_size, shdr->sh_addr, MAXSIZE);
      }
    }
  }

  return n;
}

int objhash(const pbuffer_t p0, const poptions_t o) {
  int r = 0;
  pbuffer_t p1 = NULL;
  if (o->inpname1[0]) {
    p1 = bopen(o->inpname1);
    if (p1) {
    } else {
      printf_e("'%s': no such file.", o->inpname1);
      goto objhash_die;
    }
  }

  if (isELF32(p0)) {
    dump_createELF32(p0, o);
    dump_createELF32(p1, o);
  } else if (isELF64(p0)) {
    dump_createELF64(p0, o);
    dump_createELF64(p1, o);
  }

  bfree(p1);
  return 0;

objhash_die:
  bfree(p1);
  return -1;
}

