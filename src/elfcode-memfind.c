#include "objutils.h"
#include "elfcode-endian.h"
#include "elfcode-memfind.h"

handle_t ecapply_relocs(handle_t p, handle_t q, const int index) {
  if (isfind(p) && isELF64(q)) {
    MEMSTACK(Elf64_Ehdr, ex);
    Elf64_Ehdr *ehdr = ecget_ehdr64(q, ex);
    if (ehdr) {
//printf("hunting[index] = %d\n", index);
      for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
        MEMSTACK(Elf64_Shdr, sx);
        Elf64_Shdr *s0 = ecget_shdr64byindex(q, sx, i);
        if (s0 && index == s0->sh_info) {
//printf("matched = %d with %d\n", index, i);
          size_t cnt = s0->sh_size / s0->sh_entsize;
          if (SHT_REL == s0->sh_type) {
//printf("missing[REL]\n");
          } else if (SHT_RELA == s0->sh_type) {
//printf("processing[RELA:%d:%ld:%ld:%ld]\n", s0->sh_link, s0->sh_offset, s0->sh_size, s0->sh_entsize);
            MEMSTACK(Elf64_Shdr, dx);
            Elf64_Shdr *d0 = ecget_shdr64byindex(q, dx, s0->sh_link);
            if (d0) {
              handle_t q0 = fget64byshdr(q, s0);
              if (q0) {
                for (size_t j = 0; j < cnt; ++j) {
                  MEMSTACK(Elf64_Rela, rx);
                  Elf64_Rela *r0 = ecconvert_rela64(q, rx, fgetp(q0, sizeof(Elf64_Rela)));
                  if (r0) {
                    MEMSTACK(Elf64_Sym, mx);
                    Elf64_Sym *m0 = ecconvert_sym64(q, mx, getp(q, d0->sh_offset + (ELF64_R_SYM(r0->r_info) * d0->sh_entsize), d0->sh_entsize));
//printf("offset[RELA] = 0x%lx|0x%lx:0x%lx:0x%lx|0x%lx|0x%lx\n", r0->r_offset, r0->r_info, ELF64_R_TYPE(r0->r_info), ELF64_R_SYM(r0->r_info), m0->st_value, r0->r_addend);
                    if (isused(get_RELTYPESYM32(q), ELF64_R_TYPE(r0->r_info))) {
                      if (m0 && m0->st_value) {
//printf("sym32 0x%lx\n", m0->st_value);
                        fsetu32byoffset(p, r0->r_offset, m0->st_value);
                      }
                    } else if (isused(get_RELTYPESYM64(q), ELF64_R_TYPE(r0->r_info))) {
                      if (m0 && m0->st_value) {
//printf("sym64 0x%lx\n", m0->st_value);
                        fsetu64byoffset(p, r0->r_offset, m0->st_value);
                      }
                    } else if (isused(get_RELTYPESHEX8(q), ELF64_R_TYPE(r0->r_info))) {
                      if (r0->r_addend) {
//printf("shex8 0x%lx\n", r0->r_addend);
                        fsetu8byoffset(p, r0->r_offset, r0->r_addend);
                      }
                    } else if (isused(get_RELTYPESHEX16(q), ELF64_R_TYPE(r0->r_info))) {
                      if (r0->r_addend) {
//printf("shex16 0x%lx\n", r0->r_addend);
                        fsetu16byoffset(p, r0->r_offset, r0->r_addend);
                      }
                    } else if (isused(get_RELTYPESHEX32(q), ELF64_R_TYPE(r0->r_info))) {
                      if (r0->r_addend) {
//printf("shex32 0x%lx\n", r0->r_addend);
                        fsetu32byoffset(p, r0->r_offset, r0->r_addend);
                      }
                    } else if (isused(get_RELTYPESHEX64(q), ELF64_R_TYPE(r0->r_info))) {
                      if (r0->r_addend) {
//printf("shex64 0x%lx\n", r0->r_addend);
                        fsetu64byoffset(p, r0->r_offset, r0->r_addend);
                      }
                    }
                  }
                }

                ffree(q0);
              }
            }
          } else if (SHT_RELR == s0->sh_type) {
//printf("missing[RELR]\n");
          }
        }
      }
    }

    return freset(p);
  }

  return isfind(p) ? p : NULL;
}

handle_t ecapply_relocsbyoffset(handle_t p, handle_t q, const int offset) {
  if (isfind(p) && isELF64(q)) {
    MEMSTACK(Elf64_Ehdr, ex);
    Elf64_Ehdr *e0 = ecget_ehdr64(q, ex);
    if (e0) {
//printf("hunting[offset] = %x\n", offset);
      for (Elf64_Half i = 0; i < e0->e_shnum; ++i) {
        MEMSTACK(Elf64_Shdr, sx);
        Elf64_Shdr *s0 = ecget_shdr64byindex(q, sx, i);
        if (s0 && offset == s0->sh_offset) {
          return ecapply_relocs(p, q, i);
        }
      }
    }
  }

  return isfind(p) ? p : NULL;
}

