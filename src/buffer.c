#include <stdio.h>
#include <malloc.h>

#include "buffer.h"

pbuffer_t create(const char* name) {
  FILE* f = fopen(name, "rb");
  if (f) {
    pbuffer_t p = malloc(sizeof(buffer_t));

    fseek(f, 0, SEEK_END);
    p->size = ftell(f);
    fseek(f, 0, SEEK_SET);

    p->data = malloc(p->size);
    if (p->size != fread(p->data, 1, p->size, f)) {
      p = destroy(p);
    }

    fclose(f);
    return p;
  }

  return 0;
}

pbuffer_t destroy(pbuffer_t p) {
  if (p) {
    free(p->data);
    free(p);
  }

  return NULL;
}

int issafe(pbuffer_t p) {
  return p && p->data;
}

void* getp(const pbuffer_t p, const int index, const size_t size) {
  if (issafe(p) && (index + size - 1) < p->size) {
    return ((unsigned char*)p->data) + index;
  }

  return NULL;
}

int get(const pbuffer_t p, const int index) {
  unsigned char *v = getp(p, index, 1);
  if (v) {
    return *v;
  }

  return -1;
}

Elf32_Ehdr* get_ehdr32(const pbuffer_t p) {
  return (Elf32_Ehdr*)getp(p, 0, sizeof(Elf32_Ehdr));
}

Elf64_Ehdr* get_ehdr64(const pbuffer_t p) {
  return (Elf64_Ehdr*)getp(p, 0, sizeof(Elf64_Ehdr));
}

Elf64_Shdr* get_shdr64(const pbuffer_t p, const int index) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    return (Elf64_Shdr*)getp(p, e->e_shoff + (e->e_shentsize * index), e->e_shentsize);
  }

  return NULL;
}

Elf64_Phdr* get_phdr64(const pbuffer_t p, const int index) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    return (Elf64_Phdr*)getp(p, e->e_phoff + (e->e_phentsize * index), e->e_phentsize);
  }

  return NULL;
}

int is32(const pbuffer_t p) {
  if (issafe(p)) {
    return 1 == get(p, EI_CLASS) ? 1 : 0;
  }

  return -1;
}

int is64(const pbuffer_t p) {
  if (issafe(p)) {
    return 2 == get(p, EI_CLASS) ? 1 : 0;
  }

  return -1;
}

int isELF(const pbuffer_t p) {
  if (issafe(p)) {
    return 0x7f == get(p, EI_MAG0) && 'E' == get(p, EI_MAG1) && 'L' == get(p, EI_MAG2) && 'F' == get(p, EI_MAG3) ? 1 : 0;
  }

  return -1;
}

int isBigEndian(const pbuffer_t p) {
  if (issafe(p)) {
    return ELFDATA2MSB == get(p, EI_DATA);
  }

  return -1;
}

int isLittleEndian(const pbuffer_t p) {
  if (issafe(p)) {
    return ELFDATA2LSB == get(p, EI_DATA);
  }

  return -1;
}

