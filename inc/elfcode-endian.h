#ifndef __ELFCODE_ENDIAN_H_
#define __ELFCODE_ENDIAN_H_

#include "elfcode.h"

Elf32_Dyn* ecconvert_dyn32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Dyn* ecconvert_dyn64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Ehdr* ecconvert_ehdr32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Ehdr* ecconvert_ehdr64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Phdr* ecconvert_phdr32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Phdr* ecconvert_phdr64(const pbuffer_t p, unknown_t dst, unknown_t src);

Elf32_Shdr* ecconvert_shdr32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Shdr* ecconvert_shdr64(const pbuffer_t p, unknown_t dst, unknown_t src);

#endif

