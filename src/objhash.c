#include <string.h>

#include "printf.h"
#include "elfcode.h"
#include "objhash.h"

static int dump_create0(const pbuffer_t p, const int maxsize) {
  int n = 0;
  n += printf_text("SHA-256", USE_LT | SET_PAD(64));
  n += printf_text("Name", USE_LT | USE_SPACE | SET_PAD(maxsize));
  n += printf_text("Type", USE_LT | USE_SPACE | SET_PAD(16));
  n += printf_text("Address", USE_LT | USE_SPACE | SET_PAD(isELF64(p) ? 17 : 9));
  n += printf_text("Offset   Size", USE_LT | USE_SPACE | USE_EOL);

  return n;
}

static int dump_create1(const pbuffer_t p, const poptions_t o, const char* name,
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
  const int MAXSIZE = MAX(get_secnamemaxsize(p) + 1, 21);

  int n = 0;
  Elf32_Ehdr *ehdr = get_ehdr32(p);
  if (ehdr) {
    n += dump_create0(p, MAXSIZE);

    for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
      Elf32_Shdr *shdr = get_shdr32byindex(p, i);
      if (shdr) {
        n += dump_create1(p, o, get_secnamebyindex(p, i), shdr->sh_type, shdr->sh_offset, shdr->sh_size, shdr->sh_addr, MAXSIZE);
      }
    }

    n += printf_eol();
  }

  return n;
}

static int dump_createELF64(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = MAX(get_secnamemaxsize(p) + 1, 21);

  int n = 0;
  Elf64_Ehdr *ehdr = get_ehdr64(p);
  if (ehdr) {
    n += dump_create0(p, MAXSIZE);

    for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
      Elf64_Shdr *shdr = get_shdr64byindex(p, i);
      if (shdr) {
        n += dump_create1(p, o, get_secnamebyindex(p, i), shdr->sh_type, shdr->sh_offset, shdr->sh_size, shdr->sh_addr, MAXSIZE);
      }
    }

    n += printf_eol();
  }

  return n;
}

static int dump_actionsELF0(const pbuffer_t p, const poptions_t o, const char* name, const int action,
                            const uint64_t sh_type, const uint64_t sh_offset, const uint64_t sh_size, const uint64_t sh_addr) {
  int n = 0;
  if (ACT_HEXDUMP == action) {
    n += printf_text("Hex dump of section", USE_LT);
    n += printf_text(name, USE_LT | USE_SQ | USE_COLON | USE_EOL);

    if (0 != sh_size && sh_type != SHT_NOBITS) {
      n += printf_data(getp(p, sh_offset, sh_size), sh_size, sh_addr, USE_HEXDUMP);
      n += printf_eol();
      n += printf_sore(getp(p, sh_offset, sh_size), sh_size, USE_SHA256 | USE_SPACE);
      n += printf_eol();
    } else {
      printf_w("section '%s' has no data to dump!", name);
      n += printf_eol();
    }
  } else if (ACT_STRDUMP == action) {
    n += printf_text("String dump of section", USE_LT);
    n += printf_text(name, USE_LT | USE_SQ | USE_COLON | USE_EOL);

    if (0 != sh_size && sh_type != SHT_NOBITS) {
      n += printf_data(getp(p, sh_offset, sh_size), sh_size, sh_addr, USE_STRDUMP);
      n += printf_eol();
      n += printf_sore(getp(p, sh_offset, sh_size), sh_size, USE_SHA256 | USE_SPACE);
      n += printf_eol();
    } else {
      printf_w("section '%s' has no data to dump!", name);
      n += printf_eol();
    }
  }

  return n;
}


static int dump_actionsELF32(const pbuffer_t p, const poptions_t o) {
  int n = 0;
  if (issafe(p)) {
    paction_t x = o->actions;
    while (x) {
      Elf32_Shdr* shdr = get_shdr32byname(p, x->secname);
      if (shdr) {
        n += dump_actionsELF0(p, o, x->secname, x->action, shdr->sh_type, shdr->sh_offset, shdr->sh_size, shdr->sh_addr);
      } else {
        printf_w("section '%s' was not dumped because it does not exist!", x->secname);
      }

      x = x->actions;
    }
  }

  return n;
}

static int dump_actionsELF64(const pbuffer_t p, const poptions_t o) {
  int n = 0;
  if (issafe(p)) {
    paction_t x = o->actions;
    while (x) {
      Elf64_Shdr* shdr = get_shdr64byname(p, x->secname);
      if (shdr) {
        n += dump_actionsELF0(p, o, x->secname, x->action, shdr->sh_type, shdr->sh_offset, shdr->sh_size, shdr->sh_addr);
      } else {
        printf_w("section '%s' was not dumped because it does not exist!", x->secname);
      }

      x = x->actions;
    }
  }

  return n;
}

int objhash(const pbuffer_t p0, const poptions_t o) {
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
    if (o->action & OPTOBJHASH_SECTIONS)         dump_createELF32(p0, o);
    if (o->action & OPTOBJHASH_SECTIONS)         dump_createELF32(p1, o);
    if (o->actions)                              dump_actionsELF32(p0, o);
    if (o->actions)                              dump_actionsELF32(p1, o);
  } else if (isELF64(p0)) {
    if (o->action & OPTOBJHASH_SECTIONS)         dump_createELF64(p0, o);
    if (o->action & OPTOBJHASH_SECTIONS)         dump_createELF64(p1, o);
    if (o->actions)                              dump_actionsELF64(p0, o);
    if (o->actions)                              dump_actionsELF64(p1, o);
  }

  bfree(p1);
  return 0;

objhash_die:
  bfree(p1);
  return -1;
}

