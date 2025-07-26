#ifndef __OPCODE_EXAMINE_H_
#define __OPCODE_EXAMINE_H_

#include "opcode.h"

#define MODE_OCEXAMINE                (MODE_PUT0('O') | MODE_PUT1('C') | MODE_PUT2('E'))

typedef struct ocmnemonic_s {
  char       data[160];
} ocmnemonic_t, *pocmnemonic_t;

typedef struct ocoperand_s {
  char       data[160];
  uint64_t   cvalue;
  union {
    int64_t  ivalue;
    uint64_t uvalue;
  };
} ocoperand_t, *pocoperand_t;

typedef struct ocexamine_s {
  smode_t    mode;
  uint64_t   vaddr;
  char       comment[160];

  pocmnemonic_t mc;
  pocoperand_t  op1;
  pocoperand_t  op2;
} ocexamine_t, *pocexamine_t;

bool_t isocexamine(handle_t p);

handle_t oemalloc();
handle_t oefree(handle_t p);
handle_t oecreate(const uint64_t vaddr, unknown_t mnemonic, unknown_t operands);

#endif

