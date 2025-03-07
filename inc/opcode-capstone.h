#ifndef __OPCODE_CAPSTONE_H_
#define __OPCODE_CAPSTONE_H_

#include "defines.h"

int capstone_open(handle_t p, handle_t o);
int capstone_close(handle_t p);

int capstone_run(handle_t p, handle_t s);

#endif

