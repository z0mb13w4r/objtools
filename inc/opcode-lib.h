#ifndef __OPCODE_LIB_H_
#define __OPCODE_LIB_H_

#include "defines.h"

int opcodelib_open(handle_t p, handle_t o);
int opcodelib_close(handle_t p);

int opcodelib_run(handle_t p, handle_t s);
int opcodelib_raw(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr);

#endif

