#include "opcode-examine.h"

bool_t isocexamine(handle_t p) {
  return ismode(p, MODE_OCEXAMINE);
}

handle_t oemalloc() {
  pocexamine_t p = xmalloc(sizeof(ocexamine_t));
  if (p) {
  }

  return setmode(p, MODE_OCEXAMINE);
}

handle_t oefree(handle_t p) {
  if (isocexamine(p)) {
    free(p);
    return NULL;
  }

  return p;
}

handle_t oecreate(const uint64_t vaddr, unknown_t mnemonic, unknown_t opcodes) {
  pocexamine_t p = oemalloc();
  if (p) {
    p->vaddr = vaddr;
  }

  return p;
}

