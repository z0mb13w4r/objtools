#include "elfcode.h"
#include "memfind.h"
#include "opcode-elf.h"
#include "elfcode-endian.h"

int opcodeelf_dynamics(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  if (isopcode(p) && cbfunc) {
    handle_t p0 = ocget(p, OPCODE_RAWDATA);
    if (isELF32(p0)) {
      MEMSTACK(Elf32_Ehdr, ex);
      Elf32_Ehdr *e0 = ecget_ehdr32(p0, ex);
      if (e0) {
        for (Elf32_Half i = 0; i < e0->e_shnum; ++i) {
          MEMSTACK(Elf32_Shdr, sx);
          Elf32_Shdr *s0 = ecget_shdr32byindex(p0, sx, i);
          if (s0 && SHT_DYNAMIC == s0->sh_type) {
            size_t cnt = s0->sh_size / s0->sh_entsize;
            handle_t p1 = fget32byshdr(p0, s0);
            if (p1) {
              for (size_t j = 0; j < cnt; ++j) {
                MEMSTACK(Elf32_Dyn, dx);
                Elf32_Dyn *d0 = ecconvert_dyn32(p0, dx, fgetp(p1, sizeof(Elf32_Dyn)));
                if (d0) {
                  MALLOCSPARAMS(opwrap_t, oc, MODE_OCDHDR32, d0, s0, p0);
                  cbfunc(p, poc, param);
                }
              }

              ffree(p1);
            }
          }
        }
      }
    } else if (isELF64(p0)) {
      MEMSTACK(Elf64_Ehdr, ex);
      Elf64_Ehdr *e0 = ecget_ehdr64(p0, ex);
      if (e0) {
        for (Elf64_Half i = 0; i < e0->e_shnum; ++i) {
          MEMSTACK(Elf64_Shdr, sx);
          Elf64_Shdr *s0 = ecget_shdr64byindex(p0, sx, i);
          if (s0 && SHT_DYNAMIC == s0->sh_type) {
            size_t cnt = s0->sh_size / s0->sh_entsize;
            handle_t p1 = fget64byshdr(p0, s0);
            if (p1) {
              for (size_t j = 0; j < cnt; ++j) {
                MEMSTACK(Elf64_Dyn, dx);
                Elf64_Dyn *d0 = ecconvert_dyn64(p0, dx, fgetp(p1, sizeof(Elf64_Dyn)));
                if (d0) {
                  MALLOCSPARAMS(opwrap_t, oc, MODE_OCDHDR64, d0, s0, p0);
                  cbfunc(p, poc, param);
                }
              }

              ffree(p1);
            }
          }
        }
      }
    }
  }

  return ECODE_HANDLE;
}

int opcodeelf_programs(handle_t p, opcbfunc_t cbfunc, unknown_t param) {
  if (isopcode(p) && cbfunc) {
    handle_t p0 = ocget(p, OPCODE_RAWDATA);

    if (isELF32(p0)) {
      MEMSTACK(Elf32_Ehdr, ex);
      Elf32_Ehdr *ehdr = ecget_ehdr32(p0, ex);
      if (ehdr) {
        for (Elf32_Half i = 0; i < ehdr->e_phnum; ++i) {
          MEMSTACK(Elf32_Phdr, px);
          Elf32_Phdr *p1 = ecget_phdr32byindex(p0, px, i);
          if (p1) {
            MALLOCSWRAP3(opwrap_t, oc, MODE_OCPHDR32, p1, p0, i);
            cbfunc(p, poc, param);
          }
        }

        return ECODE_OK;
      }
    } else if (isELF64(p0)) {
      MEMSTACK(Elf64_Ehdr, ex);
      Elf64_Ehdr *ehdr = ecget_ehdr64(p0, ex);
      if (ehdr) {
        for (Elf64_Half i = 0; i < ehdr->e_phnum; ++i) {
          MEMSTACK(Elf64_Phdr, px);
          Elf64_Phdr *p1 = ecget_phdr64byindex(p0, px, i);
          if (p1) {
            MALLOCSWRAP3(opwrap_t, oc, MODE_OCPHDR64, p1, p0, i);
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
      MEMSTACK(Elf32_Ehdr, ex);
      Elf32_Ehdr *e0 = ecget_ehdr32(p0, ex);
      if (e0) {
        for (Elf32_Half i = 0; i < e0->e_shnum; ++i) {
          MEMSTACK(Elf32_Shdr, sx);
          Elf32_Shdr *s0 = ecget_shdr32byindex(p0, sx, i);
          if (s0) {
            MALLOCSWRAP3(opwrap_t, oc, MODE_OCSHDR32, s0, p0, i);
            cbfunc(p, poc, param);
          }
        }

        return ECODE_OK;
      }
    } else if (isELF64(p0)) {
      MEMSTACK(Elf64_Ehdr, ex);
      Elf64_Ehdr *e0 = ecget_ehdr64(p0, ex);
      if (e0) {
        for (Elf64_Half i = 0; i < e0->e_shnum; ++i) {
          MEMSTACK(Elf64_Shdr, sx);
          Elf64_Shdr *s0 = ecget_shdr64byindex(p0, sx, i);
          if (s0) {
            MALLOCSWRAP3(opwrap_t, oc, MODE_OCSHDR64, s0, p0, i);
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

