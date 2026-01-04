#ifndef __OPCODE_CAPSTONE_H_
#define __OPCODE_CAPSTONE_H_

#include "defines.h"

int capstone_open(handle_t p, handle_t o);
int capstone_close(handle_t p);

int capstone_run(handle_t p, handle_t s);
int capstone_raw0(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr);
int capstone_raw1(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr);

#endif

