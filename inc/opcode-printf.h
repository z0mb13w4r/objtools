#ifndef __OPCODE_PRINTF_H_
#define __OPCODE_PRINTF_H_

#include "opcode.h"
#include "printf.h"

int opcode_printf_DEC(handle_t p, const uint64_t v, const imode_t mode);
int opcode_printf_FHEX(handle_t p, const uint64_t v, const imode_t mode);
int opcode_printf_LHEX(handle_t p, const uint64_t v, const imode_t mode);
int opcode_printf_FADDR(handle_t p, const uint64_t v, const imode_t mode);
int opcode_printf_LADDR(handle_t p, const uint64_t v, const imode_t mode);

int opcode_printf_pick(handle_t p, const pconvert_t z, const pick_t x, const imode_t mode);

int opcode_printf_source(handle_t p, const uint64_t vaddr);
int opcode_printf_detail(handle_t p, const uint64_t vaddr, unknown_t mnemonic, unknown_t operands);

#endif

