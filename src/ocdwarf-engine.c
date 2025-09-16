#include "ocdwarf-engine.h"

handle_t ocdwarf_create(handle_t p, handle_t e) {
  if (isopcode(p) && isocengine(e)) {
    return e;
  }

  return NULL;
}

