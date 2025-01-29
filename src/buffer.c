#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "buffer.h"
#include "options.h"

static int ismode(void *p, const int mode) {
  if (p) {
    const char* pc = p;
    if (MODE_GET0(mode) != pc[0]) return 0;
    else if (MODE_GET1(mode) != pc[1]) return 0;
    else if (MODE_GET2(mode) != pc[2]) return 0;
    else return 1;
  }

  return 0;
}

static void* setmode(void *p, const int mode) {
  if (p) {
    char* pc = p;
    pc[0] = MODE_GET0(mode);
    pc[1] = MODE_GET1(mode);
    pc[2] = MODE_GET2(mode);
  }

  return p;
}

void* mallocx(const size_t size) {
  void* p = malloc(size);
  if (p) {
    memset(p, 0, size);
  }
  return p;
}

void* create(const int mode) {
  if (MODE_BUFFER == mode) {
    pbuffer_t p = mallocx(sizeof(buffer_t));
    return setmode(p, mode);
  } else if (MODE_OPTIONS == mode) {
    poptions_t p = mallocx(sizeof(options_t));
    return setmode(p, mode);
  } else if (MODE_ACTIONS == mode) {
    paction_t p = mallocx(sizeof(action_t));
    return setmode(p, mode);
  }

  return NULL;
}

void* destroy(void* p) {
  if (p) {
    if (ismode(p, MODE_BUFFER)) {
      free(((pbuffer_t)p)->data);
    } else if (ismode(p, MODE_OPTIONS)) {
      destroy(((poptions_t)p)->actions);
    } else if (ismode(p, MODE_ACTIONS)) {
      destroy(((paction_t)p)->actions);
    }
    free(p);
  }

  return NULL;
}

pbuffer_t open(const char* name) {
  FILE* f = fopen(name, "rb");
  if (f) {
    pbuffer_t p = create(MODE_BUFFER);
    if (p) {
      fseek(f, 0, SEEK_END);
      p->size = ftell(f);
      fseek(f, 0, SEEK_SET);

      p->data = malloc(p->size);
      if (p->size != fread(p->data, 1, p->size, f)) {
        p = destroy(p);
      }

      fclose(f);
    }

    return p;
  }

  return 0;
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

