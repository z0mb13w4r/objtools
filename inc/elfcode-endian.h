#ifndef __ELFCODE_ENDIAN_H_
#define __ELFCODE_ENDIAN_H_

#include "elfcode.h"

uint16_t ecconvert_u16(const pbuffer_t p, const uint16_t v);
uint32_t ecconvert_u32(const pbuffer_t p, const uint32_t v);
uint64_t ecconvert_u64(const pbuffer_t p, const uint64_t v);

Elf32_Dyn* ecconvert_dyn32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Dyn* ecconvert_dyn64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Ehdr* ecconvert_ehdr32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Ehdr* ecconvert_ehdr64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Nhdr* ecconvert_nhdr32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Nhdr* ecconvert_nhdr64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Phdr* ecconvert_phdr32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Phdr* ecconvert_phdr64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Rel* ecconvert_rel32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Rel* ecconvert_rel64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Rela* ecconvert_rela32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Rela* ecconvert_rela64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Shdr* ecconvert_shdr32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Shdr* ecconvert_shdr64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Sym* ecconvert_sym32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Sym* ecconvert_sym64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Vernaux* ecconvert_vernaux32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Vernaux* ecconvert_vernaux64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Verneed* ecconvert_verneed32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Verneed* ecconvert_verneed64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Versym* ecconvert_versym32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Versym* ecconvert_versym64(const pbuffer_t p, unknown_t dst, unknown_t src);

#endif

