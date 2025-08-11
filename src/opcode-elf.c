#include "elfcode.h"
#include "opcode-elf.h"

int opcodeelf_programs(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  if (isopcode(p) && cbfunc) {
    handle_t p0 = ocget(p, OPCODE_RAWDATA);

    if (isELF32(p0)) {
      Elf32_Ehdr *ehdr = ecget_ehdr32(p0);
      if (ehdr) {
        for (Elf32_Half i = 0; i < ehdr->e_phnum; ++i) {
          Elf32_Phdr *phdr = ecget_phdr32byindex(p0, i);
          if (phdr) {
            MALLOCSWRAP(opwrap_t, oc, MODE_OCPHDR32, phdr);
            cbfunc(p, poc, param);
          }
        }

        return ECODE_OK;
      }
    } else if (isELF64(p0)) {
      Elf64_Ehdr *ehdr = ecget_ehdr64(p0);
      if (ehdr) {
        for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i) {
          Elf64_Phdr *phdr = ecget_phdr64byindex(p0, i);
          if (phdr) {
            MALLOCSWRAP(opwrap_t, oc, MODE_OCPHDR64, phdr);
            cbfunc(p, poc, param);
          }
        }

        return ECODE_OK;
      }
    }
  }

  return ECODE_HANDLE;
}

int opcodeelf_sections(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  if (isopcode(p) && cbfunc) {
    handle_t p0 = ocget(p, OPCODE_RAWDATA);

    if (isELF32(p0)) {
      Elf32_Ehdr *ehdr = ecget_ehdr32(p0);
      if (ehdr) {
        for (Elf32_Half i = 0; i < ehdr->e_shnum; ++i) {
          Elf32_Shdr *shdr = ecget_shdr32byindex(p0, i);
          if (shdr) {
            MALLOCSWRAP(opwrap_t, oc, MODE_OCSHDR32, shdr);
            cbfunc(p, poc, param);
          }
        }

        return ECODE_OK;
      }
    } else if (isELF64(p0)) {
      Elf64_Ehdr *ehdr = ecget_ehdr64(p0);
      if (ehdr) {
        for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
          Elf64_Shdr *shdr = ecget_shdr64byindex(p0, i);
          if (shdr) {
            MALLOCSWRAP(opwrap_t, oc, MODE_OCSHDR64, shdr);
            cbfunc(p, poc, param);
          }
        }

        return ECODE_OK;
      }
    }
  }

  return ECODE_HANDLE;
}

char* opcodeelf_getsymbol(handle_t p, const uint64_t vaddr, uint64_t *offset) {
  if (isopcode(p)) {
    handle_t p0 = ocget(p, OPCODE_RAWDATA);
    return CAST(char*, ecget_namebyaddr(p0, vaddr, offset));
  }

  return NULL;
}

