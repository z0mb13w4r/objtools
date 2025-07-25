#ifndef __OPCODE_EXAMINE_H_
#define __OPCODE_EXAMINE_H_

#include "opcode.h"

#define MODE_OCEXAMINE                (MODE_PUT0('O') | MODE_PUT1('C') | MODE_PUT2('E'))

typedef struct ocexamine_s {
  smode_t    mode;
  uint64_t   vaddr;
} ocexamine_t, *pocexamine_t;

bool_t isocexamine(handle_t p);

handle_t oemalloc();
handle_t oefree(handle_t p);
handle_t oecreate(const uint64_t vaddr, unknown_t mnemonic, unknown_t opcodes);

#endif

