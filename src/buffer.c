#include <string.h>
#include <malloc.h>

#include "buffer.h"
#include "options.h"
#include "memlink.h"
#include "objutils.h"

static int ismode0(void *p, const int mode) {
  if (p) {
    const char* pc = p;
    if (MODE_GET0(mode) != pc[0])      return 0;
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
    pc[3] = MODE_GET3(mode);
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

handle_t create(const int mode) {
  if (MODE_BUFFER == (mode & MODE_MASK0)) {
    pbuffer_t p = mallocx(sizeof(buffer_t));
    return setmode(p, mode);
  } else if (MODE_OPTIONS == (mode & MODE_MASK0)) {
    poptions_t p = mallocx(sizeof(options_t));
    return setmode(p, mode);
  } else if (MODE_ACTIONS == (mode & MODE_MASK0)) {
    paction_t p = mallocx(sizeof(action_t));
    return setmode(p, mode);
  } else if (MODE_LINKS == (mode & MODE_MASK0)) {
    paction_t p = mallocx(sizeof(node_t));
    return setmode(p, mode);
  }

  return NULL;
}

void* createx(const pbuffer_t p, const int mode) {
  pbuffer_t pp = p;
  while (pp) {
    if (ismode(pp, mode)) return pp;
    if (NULL == pp->next) pp->next = create(mode);

    pp = pp->next;
  }

  return create(mode);
}

handle_t destroy(handle_t p) {
  if (p) {
    if (ismode0(p, MODE_BUFFER)) {
      destroy(CAST(pbuffer_t, p)->next);
      free(CAST(pbuffer_t, p)->data);
    } else if (ismode0(p, MODE_OPTIONS)) {
      destroy(CAST(poptions_t, p)->actions);
    } else if (ismode0(p, MODE_ACTIONS)) {
      destroy(CAST(paction_t, p)->actions);
    } else if (ismode0(p, MODE_LINKS)) {
      destroy(CAST(pnode_t, p)->item);
    }

    free(p);
  }

  return NULL;
}

handle_t release(handle_t p) {
  if (ismode0(p, MODE_LINKS)) {
    free(p);
  }

  return NULL;
}

pbuffer_t open(const char* name) {
  FILE* f = fopen(name, "rb");
  if (f) {
    pbuffer_t p = create(MODE_BUFFER);
    if (p) {
      strname(p->note, name);
      p->size = fsize(f);
      p->data = mallocx(p->size);
      if (p->size != fread(p->data, 1, p->size, f)) {
        p = destroy(p);
      }

      fclose(f);
    }

    return p;
  }

  return 0;
}

int ismode(handle_t p, const int mode) {
  if (p) {
    const char* pc = p;
    if (MODE_GET0(mode) != pc[0]) return 0;
    if (MODE_GET1(mode) != pc[1]) return 0;
    if (MODE_GET2(mode) != pc[2]) return 0;
    if (MODE_GET3(mode) != pc[3]) return 0;
    return 1;
  }

  return 0;
}

int issafe(pbuffer_t p) {
  return p && p->data;
}

void* get64byshdr(const pbuffer_t p, Elf64_Shdr *shdr) {
  if (shdr) {
    return getp(p, shdr->sh_offset, shdr->sh_size);
  }

  return NULL;
}

void* getp(const pbuffer_t p, const int offset, const size_t size) {
  if (issafe(p) && (offset + size - 1) < p->size) {
    return ((unsigned char*)p->data) + offset;
  }

  return NULL;
}

int get(const pbuffer_t p, const int offset) {
  unsigned char *v = getp(p, offset, 1);
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

Elf64_Shdr* get_shdr64byindex(const pbuffer_t p, const int index) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    return (Elf64_Shdr*)getp(p, e->e_shoff + (e->e_shentsize * index), e->e_shentsize);
  }

  return NULL;
}

Elf64_Shdr* get_shdr64bytype(const pbuffer_t p, const int type) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
      Elf64_Shdr *s = get_shdr64byindex(p, i);
      if (s && s->sh_type == type){
        return s;
      }
    }
  }

  return NULL;
}

Elf64_Shdr* get_shdr64byname(const pbuffer_t p, const char* name) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    for (Elf64_Half i = 0; i < e->e_shnum; ++i) {
      Elf64_Shdr *s = get_shdr64byindex(p, i);
      if (s && 0 == strcmp(name, get_secname64byindex(p, i))) {
        return s;
      }
    }
  }

  return NULL;
}

Elf64_Nhdr* get_nhdr64byindex(const pbuffer_t p, const int index) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    Elf64_Shdr *s = get_shdr64byindex(p, index);
    if (s && SHT_NOTE == s->sh_type) {
      return (Elf64_Nhdr*)getp(p, s->sh_offset, s->sh_size);
    }
  }

  return NULL;
}

const char* get_nhdrname64byindex(const pbuffer_t p, const int index) {
  Elf64_Nhdr *n = get_nhdr64byindex(p, index);
  if (n && n->n_namesz) {
    Elf64_Shdr *s = get_shdr64byindex(p, index);
    if (s) {
      return getp(p, s->sh_offset + sizeof(Elf64_Nhdr), n->n_namesz);
    }
  }

  return NULL;
}

const char* get_nhdrdesc64byindex(const pbuffer_t p, const int index) {
  Elf64_Nhdr *n = get_nhdr64byindex(p, index);
  if (n && n->n_namesz && n->n_descsz) {
    Elf64_Shdr *s = get_shdr64byindex(p, index);
    if (s) {
      return getp(p, s->sh_offset + sizeof(Elf64_Nhdr) + n->n_namesz, n->n_descsz);
    }
  }

  return NULL;
}


Elf64_Phdr* get_phdr64byindex(const pbuffer_t p, const int index) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    return (Elf64_Phdr*)getp(p, e->e_phoff + (e->e_phentsize * index), e->e_phentsize);
  }

  return NULL;
}

const char* get_name64byoffset(const pbuffer_t p, const int index, const int offset) {
  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    Elf64_Shdr *s = get_shdr64byindex(p, index);
    if (s) {
      return getp(p, s->sh_offset + offset, s->sh_size - offset);
    }
  }

  return NULL;
}

const char* get_secname64byindex(const pbuffer_t p, const int index) {
  char *s0 = NULL;

  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    Elf64_Shdr *s1 = get_shdr64byindex(p, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0) {
      Elf64_Shdr *s2 = get_shdr64byindex(p, index);
      if (s2) {
        return s0 + s2->sh_name;
      }
    }
  }

  return NULL;
}

const char* get_secname64byshdr(const pbuffer_t p, Elf64_Shdr *s) {
  char *s0 = NULL;

  Elf64_Ehdr *e = get_ehdr64(p);
  if (e) {
    Elf64_Shdr *s1 = get_shdr64byindex(p, e->e_shstrndx);
    if (s1) {
      s0 = getp(p, s1->sh_offset, s1->sh_size);
    }

    if (s0 && s) {
      return s0 + s->sh_name;
    }
  }

  return NULL;
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

