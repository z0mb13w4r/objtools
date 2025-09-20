#include "opcode-engine-got.h"

handle_t opcode_create(handle_t p, handle_t q) {
  if (isopcode(p) && isocengine(q)) {
    return q;
  }

  return NULL;
}

