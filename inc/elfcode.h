#ifndef __ELFCODE_H_
#define __ELFCODE_H_

#include "common.h"

int shdrinphdr64(Elf64_Shdr *s, Elf64_Phdr *p);

uint64_t getLE(const void *p, const size_t siz);
uint64_t getBE(const void *p, const size_t siz);

#endif

