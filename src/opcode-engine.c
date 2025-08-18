#include "opcode-engine.h"

bool_t isocengine(handle_t p) {
  return ismode(p, MODE_OCENGINE);
}

handle_t emalloc() {
  pocengine_t p = xmalloc(sizeof(ocengine_t));
  if (p) {
  }

  return setmode(p, MODE_OCENGINE);
}

handle_t efree(handle_t p) {
  if (isocengine(p)) {
    free(p);
    return NULL;
  }

  return p;
}

handle_t oecreate_engine(handle_t p) {
  if (isopcode(p)) {
    return emalloc();
  }

  return NULL;
}

handle_t oegetbyaddr(handle_t p, const uint64_t vaddr, const imode_t mode) {
  return NULL;
}

handle_t oeseebyaddr(handle_t p, const uint64_t vaddr, const imode_t mode) {
  return NULL;
}

