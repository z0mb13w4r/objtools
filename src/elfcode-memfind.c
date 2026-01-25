//#include "printf.h"
#include "objutils.h"
#include "elfcode-memfind.h"

handle_t ecapply_relocs(handle_t p, handle_t q, const int index) {
  if (isfind(p) && isELF64(q)) {
    Elf64_Ehdr *ehdr = ecget_ehdr64(q);
    if (ehdr) {
//printf("hunting[index] = %d\n", index);
      for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
        Elf64_Shdr *shdr = ecget_shdr64byindex(q, i);
        if (shdr && index == shdr->sh_info) {
//printf("matched = %d with %d\n", index, i);
          size_t cnt = shdr->sh_size / shdr->sh_entsize;
          if (SHT_REL == shdr->sh_type) {
//printf("missing[REL]\n");
          } else if (SHT_RELA == shdr->sh_type) {
            Elf64_Rela *r = _get64byshdr(q, shdr);
            if (r) {
              for (size_t j = 0; j < cnt; ++j, ++r) {
//printf("offset[RELA] = 0x%lx|0x%lx:0x%lx|0x%lx\n", r->r_offset, r->r_info, ELF64_R_TYPE(r->r_info), r->r_addend);
                if (isused(get_RELTYPESHEX8(q), ELF64_R_TYPE(r->r_info))) {
                  if (r->r_addend) {
//printf("shex8 0x%lx\n", r->r_addend);
                    fsetu8byoffset(p, r->r_offset, r->r_addend);
                  }
                } else if (isused(get_RELTYPESHEX16(q), ELF64_R_TYPE(r->r_info))) {
                  if (r->r_addend) {
//printf("shex16 0x%lx\n", r->r_addend);
                    fsetu16byoffset(p, r->r_offset, r->r_addend);
                  }
                } else if (isused(get_RELTYPESHEX32(q), ELF64_R_TYPE(r->r_info))) {
                  if (r->r_addend) {
//printf("shex32 0x%lx\n", r->r_addend);
                    fsetu32byoffset(p, r->r_offset, r->r_addend);
                  }
                } else if (isused(get_RELTYPESHEX64(q), ELF64_R_TYPE(r->r_info))) {
                  if (r->r_addend) {
//printf("shex64 0x%lx\n", r->r_addend);
                    fsetu64byoffset(p, r->r_offset, r->r_addend);
                  }
                }
              }
            }
          } else if (SHT_RELR == shdr->sh_type) {
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
    Elf64_Ehdr *ehdr = ecget_ehdr64(q);
    if (ehdr) {
//printf("hunting[offset] = %x\n", offset);
      for (Elf64_Half i = 0; i < ehdr->e_shnum; ++i) {
        Elf64_Shdr *shdr = ecget_shdr64byindex(q, i);
        if (shdr && offset == shdr->sh_offset) {
          return ecapply_relocs(p, q, i);
        }
      }
    }
  }

  return isfind(p) ? p : NULL;
}

