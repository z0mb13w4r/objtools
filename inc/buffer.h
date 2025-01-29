#ifndef __BUFFER_H_
#define __BUFFER_H_

#include "common.h"

#define MODE_GET0(x)            (((x) >> 0) & 0xFF)
#define MODE_GET1(x)            (((x) >> 8) & 0xFF)
#define MODE_GET2(x)            (((x) >> 16) & 0xFF)

#define MODE_PUT0(x)            ((x) << 0)
#define MODE_PUT1(x)            ((x) << 8)
#define MODE_PUT2(x)            ((x) << 16)

#define MODE_BUFFER             (MODE_PUT0('B') | MODE_PUT1('U') | MODE_PUT2('F'))
#define MODE_OPTIONS            (MODE_PUT0('O') | MODE_PUT1('P') | MODE_PUT2('T'))
#define MODE_ACTIONS            (MODE_PUT0('A') | MODE_PUT1('C') | MODE_PUT2('T'))

typedef struct buffer_s {
  char   mode[3];
  size_t size;
  void  *data;

} buffer_t, *pbuffer_t;

void* mallocx(const size_t size);
void* create(const int mode);
void* destroy(void* p);

pbuffer_t open(const char* name);

int issafe(pbuffer_t p);

void* getp(const pbuffer_t p, const int index, const size_t size);
int   get(const pbuffer_t p, const int index);

Elf32_Ehdr* get_ehdr32(const pbuffer_t p);
Elf64_Ehdr* get_ehdr64(const pbuffer_t p);
Elf64_Shdr* get_shdr64(const pbuffer_t p, const int index);
Elf64_Phdr* get_phdr64(const pbuffer_t p, const int index);

int is32(const pbuffer_t p);
int is64(const pbuffer_t p);
int isELF(const pbuffer_t p);
int isBigEndian(const pbuffer_t p);
int isLittleEndian(const pbuffer_t p);

#endif

