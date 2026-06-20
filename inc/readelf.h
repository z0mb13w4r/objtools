#ifndef __READELF_H_
#define __READELF_H_

#include "elfcode.h"
#include "options.h"

int readelf(const pbuffer_t p, const poptions_t o);
int dumpelf(const pbuffer_t p, const poptions_t o, const char* name);

#endif

