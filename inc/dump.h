#ifndef __DUMP_H_
#define __DUMP_H_

#include "buffer.h"
#include "options.h"

int dump_actions0(const pbuffer_t p, const poptions_t o, const uint64_t offset, const uint64_t size);
int dump_actions1(const pbuffer_t p, const poptions_t o, const handle_t s, const char* name, const int action,
                  const uint64_t offset, const uint64_t size, const uint64_t vaddr);

#endif

