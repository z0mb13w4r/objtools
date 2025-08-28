#ifndef __ELFCODE_H_
#define __ELFCODE_H_

#include <elf.h>
#include "buffer.h"

#define SHT_RELR                   (19) /* RELR relative relocations */

#define PT_GNU_MBIND_NUM           (4096)
#define PT_GNU_MBIND_LO            (PT_LOOS + 0x474e555)
#define PT_GNU_MBIND_HI            (PT_GNU_MBIND_LO + PT_GNU_MBIND_NUM - 1)

#define PT_GNU_PROPERTY            (PT_LOOS + 0x474e553) /* GNU property */
#define PT_GNU_SFRAME              (PT_LOOS + 0x474e554) /* SFrame stack trace information */

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

typedef unsigned short version_t, *pversion_t;

bool_t isTBSS32(Elf64_Shdr *s, Elf64_Phdr *p);
bool_t isTBSS64(Elf64_Shdr *s, Elf64_Phdr *p);

bool_t isshdrinphdr32(Elf32_Shdr *s, Elf32_Phdr *p);
bool_t isshdrinphdr64(Elf64_Shdr *s, Elf64_Phdr *p);

bool_t isELF(const pbuffer_t p);
bool_t isELF32(const pbuffer_t p);
bool_t isELF64(const pbuffer_t p);
bool_t isELFbe(const pbuffer_t p);
bool_t isELFle(const pbuffer_t p);

uint64_t ecget_etype(const pbuffer_t p);
uint64_t ecget_eflags(const pbuffer_t p);
uint64_t ecget_emachine(const pbuffer_t p);

Elf32_Ehdr* ecget_ehdr32(const pbuffer_t p);
Elf64_Ehdr* ecget_ehdr64(const pbuffer_t p);

Elf32_Nhdr* ecget_nhdr32byindex(const pbuffer_t p, const int index);
Elf64_Nhdr* ecget_nhdr64byindex(const pbuffer_t p, const int index);

Elf32_Phdr* ecget_phdr32byindex(const pbuffer_t p, const int index);
Elf64_Phdr* ecget_phdr64byindex(const pbuffer_t p, const int index);

Elf32_Shdr* ecget_shdr32byindex(const pbuffer_t p, const int index);
Elf64_Shdr* ecget_shdr64byindex(const pbuffer_t p, const int index);
unknown_t   ecget_shdrbyindex(const pbuffer_t p, const int index);

Elf32_Shdr* ecget_shdr32byname(const pbuffer_t p, const char* name);
Elf64_Shdr* ecget_shdr64byname(const pbuffer_t p, const char* name);
unknown_t   ecget_shdrbyname(const pbuffer_t p, const char* name);

Elf32_Shdr* ecget_shdr32bytype(const pbuffer_t p, const int type);
Elf64_Shdr* ecget_shdr64bytype(const pbuffer_t p, const int type);
unknown_t   ecget_shdrbytype(const pbuffer_t p, const int type);

size_t ecget_secnamemaxsize(const pbuffer_t p);
size_t _ecget_secname32maxsize(const pbuffer_t p);
size_t _ecget_secname64maxsize(const pbuffer_t p);

const char* ecget_secnamebyindex(const pbuffer_t p, const int index);
const char* _ecget_secname32byindex(const pbuffer_t p, const int index);
const char* _ecget_secname64byindex(const pbuffer_t p, const int index);

const char* ecget_secnamebyoffset(const pbuffer_t p, const int offset);
const char* _ecget_secname32byoffset(const pbuffer_t p, const int offset);
const char* _ecget_secname64byoffset(const pbuffer_t p, const int offset);

const char* ecget_secnamebyaddr(const pbuffer_t p, const int vaddr);
const char* _ecget_secname32byaddr(const pbuffer_t p, const int vaddr);
const char* _ecget_secname64byaddr(const pbuffer_t p, const int vaddr);

const char* ecget_namebyoffset(const pbuffer_t p, const int index, const int offset);
const char* _ecget_name32byoffset(const pbuffer_t p, const int index, const int offset);
const char* _ecget_name64byoffset(const pbuffer_t p, const int index, const int offset);

const char* ecget_namebyaddr(const pbuffer_t p, const int vaddr, uint64_t *offset);
const char* _ecget_name32byaddr(const pbuffer_t p, const int vaddr, uint64_t *offset);
const char* _ecget_name64byaddr(const pbuffer_t p, const int vaddr, uint64_t *offset);

const char* ecget_nhdrnamebyindex(const pbuffer_t p, const int index);
const char* _ecget_nhdrname32byindex(const pbuffer_t p, const int index);
const char* _ecget_nhdrname64byindex(const pbuffer_t p, const int index);

Elf32_Word* ecget_nhdrdesc32byindex(const pbuffer_t p, const int index);
Elf64_Word* ecget_nhdrdesc64byindex(const pbuffer_t p, const int index);

const char* ecget_secname32byshdr(const pbuffer_t p, Elf32_Shdr *s);
const char* ecget_secname64byshdr(const pbuffer_t p, Elf64_Shdr *s);

unknown_t _get32byshdr(const pbuffer_t p, Elf32_Shdr *shdr); // not safe - remove
unknown_t _get64byshdr(const pbuffer_t p, Elf64_Shdr *shdr); // replace by fgetXXbyYYYY & fnext

handle_t fgetbyshdr(const pbuffer_t p, unknown_t shdr);
handle_t fget32byshdr(const pbuffer_t p, Elf32_Shdr *shdr);
handle_t fget64byshdr(const pbuffer_t p, Elf64_Shdr *shdr);

int ecmake_versionnames32(const pbuffer_t p, pversion_t vnames, const size_t maxvnames);
int ecmake_versionnames64(const pbuffer_t p, pversion_t vnames, const size_t maxvnames);

ppick_t get_RELTYPEDEF(const pbuffer_t p);
ppick_t get_RELTYPEVER(const pbuffer_t p);

#endif

