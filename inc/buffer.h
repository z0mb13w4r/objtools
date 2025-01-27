#ifndef __BUFFER_H_
#define __BUFFER_H_

#include "common.h"

typedef struct buffer_s {
  size_t size;
  void  *data;

} buffer_t, *pbuffer_t;

pbuffer_t create(const char* name);
pbuffer_t destroy(pbuffer_t p);

int issafe(pbuffer_t p);

void* getp(const pbuffer_t p, const int index, const size_t size);
int   get(const pbuffer_t p, const int index);

Elf32_Ehdr* get_ehdr32(const pbuffer_t p);
Elf64_Ehdr* get_ehdr64(const pbuffer_t p);
Elf64_Shdr* get_shdr64(const pbuffer_t p, const int index);

int is32(const pbuffer_t p);
int is64(const pbuffer_t p);
int isELF(const pbuffer_t p);
int isBigEndian(const pbuffer_t p);
int isLittleEndian(const pbuffer_t p);

#endif

