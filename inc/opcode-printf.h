#ifndef __OPCODE_PRINTF_H_
#define __OPCODE_PRINTF_H_

#include "opcode.h"
#include "printf.h"

int opcode_printf_DEC(handle_t p, const uint64_t v, const imode_t mode);
int opcode_printf_FHEX(handle_t p, const uint64_t v, const imode_t mode);
int opcode_printf_LHEX(handle_t p, const uint64_t v, const imode_t mode);
int opcode_printf_FADDR(handle_t p, const uint64_t v, const imode_t mode);
int opcode_printf_LADDR(handle_t p, const uint64_t v, const imode_t mode);

int opcode_printf_pluck(handle_t p, const pconvert_t z, const pick_t x, const imode_t mode);

int opcode_printf_lnumbers(handle_t p, const uint64_t vaddr);

#endif

