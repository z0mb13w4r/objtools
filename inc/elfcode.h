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

typedef struct elf64_s {
  imode_t mode;
  size_t  spos, epos, size;
  union {
    Elf64_Ehdr ehdr;
    Elf64_Nhdr nhdr;
    Elf64_Phdr phdr;
    Elf64_Shdr shdr;
    Elf64_Vernaux vernaux;
    Elf64_Verneed verneed;
    Elf64_Versym  versym;
  };
} elf64_t;

int shdrinphdr64(Elf64_Shdr *s, Elf64_Phdr *p);

int isELF(const pbuffer_t p);
int is32(const pbuffer_t p);
int is64(const pbuffer_t p);

uint64_t getLE(const void *p, const size_t siz);
uint64_t getBE(const void *p, const size_t siz);

#endif

