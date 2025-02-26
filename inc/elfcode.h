#ifndef __ELFCODE_H_
#define __ELFCODE_H_

#include "buffer.h"

#define VERSYM_HIDDEN              (0x8000)
#define VERSYM_VERSION             (0x7fff)

/* These three macros disassemble and assemble a symbol table st_info field,
   which contains the symbol binding and symbol type.  The STB_ and STT_
   defines identify the binding and type.  */
#define ELF_ST_BIND(x)             ((x) >> 4)
#define ELF_ST_TYPE(x)             ((x) & 0x0f)
#define ELF_ST_INFO(x,y)           (((x) << 4) + ELF_ST_TYPE(y))

/* This macro disassembles and assembles a symbol's visibility into
   the st_other field.  The STV_ defines specify the actual visibility.  */
#define ELF_ST_VISIBILITY(x)       ((x) & 0x03)

int isTBSS32(Elf64_Shdr *s, Elf64_Phdr *p);
int isTBSS64(Elf64_Shdr *s, Elf64_Phdr *p);

int isshdrinphdr32(Elf32_Shdr *s, Elf32_Phdr *p);
int isshdrinphdr64(Elf64_Shdr *s, Elf64_Phdr *p);

int isELF(const pbuffer_t p);
int isELF32(const pbuffer_t p);
int isELF64(const pbuffer_t p);

uint64_t getLE(const void *p, const size_t siz);
uint64_t getBE(const void *p, const size_t siz);

Elf32_Ehdr* get_ehdr32(const pbuffer_t p);
Elf64_Ehdr* get_ehdr64(const pbuffer_t p);

Elf32_Phdr* get_phdr32byindex(const pbuffer_t p, const int index);
Elf64_Phdr* get_phdr64byindex(const pbuffer_t p, const int index);

Elf32_Shdr* get_shdr32byindex(const pbuffer_t p, const int index);
Elf64_Shdr* get_shdr64byindex(const pbuffer_t p, const int index);

size_t get_secname32maxsize(const pbuffer_t p);
size_t get_secname64maxsize(const pbuffer_t p);

const char* get_secname32byindex(const pbuffer_t p, const int index);
const char* get_secname64byindex(const pbuffer_t p, const int index);

#endif

