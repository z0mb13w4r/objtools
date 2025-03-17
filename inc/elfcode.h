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

bool_t isTBSS32(Elf64_Shdr *s, Elf64_Phdr *p);
bool_t isTBSS64(Elf64_Shdr *s, Elf64_Phdr *p);

bool_t isshdrinphdr32(Elf32_Shdr *s, Elf32_Phdr *p);
bool_t isshdrinphdr64(Elf64_Shdr *s, Elf64_Phdr *p);

bool_t isELF(const pbuffer_t p);
bool_t isELF32(const pbuffer_t p);
bool_t isELF64(const pbuffer_t p);

uint64_t get_etype(const pbuffer_t p);
uint64_t get_eflags(const pbuffer_t p);
uint64_t get_emachine(const pbuffer_t p);

uint64_t getLE(const void *p, const size_t siz);
uint64_t getBE(const void *p, const size_t siz);

Elf32_Ehdr* get_ehdr32(const pbuffer_t p);
Elf64_Ehdr* get_ehdr64(const pbuffer_t p);

Elf32_Nhdr* get_nhdr32byindex(const pbuffer_t p, const int index);
Elf64_Nhdr* get_nhdr64byindex(const pbuffer_t p, const int index);

Elf32_Phdr* get_phdr32byindex(const pbuffer_t p, const int index);
Elf64_Phdr* get_phdr64byindex(const pbuffer_t p, const int index);

Elf32_Shdr* get_shdr32bytype(const pbuffer_t p, const int type);
Elf64_Shdr* get_shdr64bytype(const pbuffer_t p, const int type);

Elf32_Shdr* get_shdr32byname(const pbuffer_t p, const char* name);
Elf64_Shdr* get_shdr64byname(const pbuffer_t p, const char* name);

Elf32_Shdr* get_shdr32byindex(const pbuffer_t p, const int index);
Elf64_Shdr* get_shdr64byindex(const pbuffer_t p, const int index);

size_t get_secnamemaxsize(const pbuffer_t p);
size_t _get_secname32maxsize(const pbuffer_t p);
size_t _get_secname64maxsize(const pbuffer_t p);

const char* get_secnamebyindex(const pbuffer_t p, const int index);
const char* _get_secname32byindex(const pbuffer_t p, const int index);
const char* _get_secname64byindex(const pbuffer_t p, const int index);

const char* get_namebyoffset(const pbuffer_t p, const int index, const int offset);
const char* _get_name32byoffset(const pbuffer_t p, const int index, const int offset);
const char* _get_name64byoffset(const pbuffer_t p, const int index, const int offset);

const char* get_nhdrnamebyindex(const pbuffer_t p, const int index);
const char* _get_nhdrname32byindex(const pbuffer_t p, const int index);
const char* _get_nhdrname64byindex(const pbuffer_t p, const int index);

Elf32_Word* get_nhdrdesc32byindex(const pbuffer_t p, const int index);
Elf64_Word* get_nhdrdesc64byindex(const pbuffer_t p, const int index);

const char* get_secname32byshdr(const pbuffer_t p, Elf32_Shdr *s);
const char* get_secname64byshdr(const pbuffer_t p, Elf64_Shdr *s);

const char* get_secnamebyoffset(const pbuffer_t p, const int offset);
const char* _get_secname32byoffset(const pbuffer_t p, const int offset);
const char* _get_secname64byoffset(const pbuffer_t p, const int offset);

void* _get32byshdr(const pbuffer_t p, Elf32_Shdr *shdr); // not safe - remove
void* _get64byshdr(const pbuffer_t p, Elf64_Shdr *shdr); // replace by fgetXXbyYYYY & fnext

handle_t fget32byshdr(const pbuffer_t p, Elf32_Shdr *shdr);
handle_t fget64byshdr(const pbuffer_t p, Elf64_Shdr *shdr);

#endif

