#include "elfcode.h"
#include "opcode-elf.h"

int opcodeelf_programs(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  if (isELF32(p)) {
    Elf32_Ehdr *ehdr = get_ehdr32(p);
    if (ehdr) {
      for (Elf32_Half i = 0; i < ehdr->e_phnum; ++i) {
        Elf32_Phdr *phdr = get_phdr32byindex(p, i);
        if (phdr) {
          MALLOCSWRAP(opwrap_t, oc, MODE_OCPHDR32, phdr);
          cbfunc(p, poc, param);
        }
      }

      return 0;
    }
  } else if (isELF64(p)) {
    Elf64_Ehdr *ehdr = get_ehdr64(p);
    if (ehdr) {
      for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i) {
        Elf64_Phdr *phdr = get_phdr64byindex(p, i);
        if (phdr) {
          MALLOCSWRAP(opwrap_t, oc, MODE_OCPHDR64, phdr);
          cbfunc(p, poc, param);
        }
      }

      return 0;
    }
  }

  return -1;
}

int opcodeelf_sections(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  if (isELF32(p)) {
    Elf32_Ehdr *ehdr = get_ehdr32(p);
    if (ehdr) {
      for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
        Elf32_Shdr *shdr = get_shdr32byindex(p, i);
        if (shdr) {
          MALLOCSWRAP(opwrap_t, oc, MODE_OCSHDR32, shdr);
          cbfunc(p, poc, param);
        }
      }

      return 0;
    }
  } else if (isELF64(p)) {
    Elf64_Ehdr *ehdr = get_ehdr64(p);
    if (ehdr) {
      for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
        Elf64_Shdr *shdr = get_shdr64byindex(p, i);
        if (shdr) {
          MALLOCSWRAP(opwrap_t, oc, MODE_OCSHDR64, shdr);
          cbfunc(p, poc, param);
        }
      }

      return 0;
    }
  }

  return -1;
}

