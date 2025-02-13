#ifndef __ELFCODE_H_
#define __ELFCODE_H_

#include "buffer.h"

#define VERSYM_HIDDEN              (0x8000)
#define VERSYM_VERSION             (0x7fff)

int shdrinphdr64(Elf64_Shdr *s, Elf64_Phdr *p);

int isELF(const pbuffer_t p);

uint64_t getLE(const void *p, const size_t siz);
uint64_t getBE(const void *p, const size_t siz);

#endif

