#include <bfd.h>

#include "ocdwarf.h"
#include "options.h"

#include "static/dwarf.ci"

int ocdwarf_isneeded(handle_t s, handle_t o) {
  if (isopsection(s) && isoptions(o)) {
    if (0 == (ocget_flags(s) & SEC_HAS_CONTENTS)) return 0;
    else if (0 == ocget_size(s)) return 0;

    return 1;
  }
  return 0;
}

int ocdwarf_open(handle_t p, handle_t o) {
  return 0;
}

int ocdwarf_close(handle_t p) {
  return 0;
}

int ocdwarf_run(handle_t p, handle_t s) {
  return 0;
}

