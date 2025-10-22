#ifndef __ELFCODE_MEMFIND_H_
#define __ELFCODE_MEMFIND_H_

#include "elfcode.h"
#include "memfind.h"

handle_t ecapply_relocs(handle_t p, handle_t q, const int index);
handle_t ecapply_relocsbyoffset(handle_t p, handle_t q, const int offset);

#endif

