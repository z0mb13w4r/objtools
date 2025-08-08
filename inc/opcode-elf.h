#ifndef __OPCODE_ELF_H_
#define __OPCODE_ELF_H_

#include "opcode.h"

int opcodeelf_programs(handle_t p, opcbfunc_t cbfunc, unknown_t param);
int opcodeelf_sections(handle_t p, opcbfunc_t cbfunc, unknown_t param);

char* opcodeelf_getsymbol(handle_t p, uint64_t vaddr);

#endif

