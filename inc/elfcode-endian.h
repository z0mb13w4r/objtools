#ifndef __ELFCODE_ENDIAN_H_
#define __ELFCODE_ENDIAN_H_

#include "elfcode.h"

Elf32_Ehdr* ecconvert_ehdr32(const pbuffer_t p, unknown_t dst, unknown_t src);
Elf64_Ehdr* ecconvert_ehdr64(const pbuffer_t p, unknown_t dst, unknown_t src);

#endif

