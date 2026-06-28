#ifndef __DUMPELF_H_
#define __DUMPELF_H_

#include "elfcode.h"
#include "options.h"

int dumpelf_archeader(const pbuffer_t p, const poptions_t o, const char* name);
int dumpelf_elfheader(const pbuffer_t p, const poptions_t o);

int dumpelf_fileheader32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_fileheader64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_sectionheaders32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_sectionheaders64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_sectiongroups32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_sectiongroups64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_programheaders32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_programheaders64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_dynamic32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_dynamic64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_relocs32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_relocs64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_unwind32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_unwind64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_symbols32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_symbols64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_histogram32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_histogram64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_version32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_version64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_actions32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_actions64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_notes32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_notes64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_archspecific32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_archspecific64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

int dumpelf_dwarf32(const pbuffer_t p, const poptions_t o, Elf32_Ehdr *ehdr);
int dumpelf_dwarf64(const pbuffer_t p, const poptions_t o, Elf64_Ehdr *ehdr);

#endif

