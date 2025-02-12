#ifndef __BUFFER_H_
#define __BUFFER_H_

#include "common.h"

#define MODE_MASK0              (0x00FFFFFF)
#define MODE_MASK1              (0xFF000000)
#define MODE_MASK               (MODE_MASK0 | MODE_MASK1)

#define MODE_GET0(x)            (((x) >>  0) & 0xFF)
#define MODE_GET1(x)            (((x) >>  8) & 0xFF)
#define MODE_GET2(x)            (((x) >> 16) & 0xFF)
#define MODE_GET3(x)            (((x) >> 24) & 0xFF)

#define MODE_PUT0(x)            ((x) <<  0)
#define MODE_PUT1(x)            ((x) <<  8)
#define MODE_PUT2(x)            ((x) << 16)
#define MODE_PUT3(x)            ((x) << 24)

#define MODE_BUFFER             (MODE_PUT0('B') | MODE_PUT1('U') | MODE_PUT2('F'))
#define MODE_OPTIONS            (MODE_PUT0('O') | MODE_PUT1('P') | MODE_PUT2('T'))
#define MODE_ACTIONS            (MODE_PUT0('A') | MODE_PUT1('C') | MODE_PUT2('T'))

#define MODE_SYMBOLS            (MODE_BUFFER | MODE_PUT3('S'))
#define MODE_SYMBOLS_DYNAMIC    (MODE_BUFFER | MODE_PUT3('D'))

typedef struct buffer_s {
  char   mode[4];
  size_t size;
  void  *data;
  struct buffer_s *next;

} buffer_t, *pbuffer_t;

void* create(const int mode);
void* createx(const pbuffer_t p, const int mode);
void* mallocx(const size_t size);
void* destroy(void* p);

pbuffer_t open(const char* name);

int issafe(pbuffer_t p);
int ismode(void *p, const int mode);

void* get64s(const pbuffer_t p, Elf64_Shdr *s);
void* getp(const pbuffer_t p, const int offset, const size_t size);
int   get(const pbuffer_t p, const int offset);

Elf32_Ehdr* get_ehdr32(const pbuffer_t p);
Elf64_Ehdr* get_ehdr64(const pbuffer_t p);

Elf64_Shdr* get_shdr64byindex(const pbuffer_t p, const int index);
Elf64_Shdr* get_shdr64byname(const pbuffer_t p, const char* name);

Elf64_Nhdr* get_nhdr64byindex(const pbuffer_t p, const int index);
const char* get_nhdrname64byindex(const pbuffer_t p, const int index);
const char* get_nhdrdesc64byindex(const pbuffer_t p, const int index);

Elf64_Phdr* get_phdr64byindex(const pbuffer_t p, const int index);

const char* get_secname64byindex(const pbuffer_t p, const int index);

int is32(const pbuffer_t p);
int is64(const pbuffer_t p);
int isBigEndian(const pbuffer_t p);
int isLittleEndian(const pbuffer_t p);

#endif

