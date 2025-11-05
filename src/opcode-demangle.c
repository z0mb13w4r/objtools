#include "options.h"
#include "opcode-demangle.h"

const char* opcode_demangle(handle_t p, const char* name) {
  STATICA(char, name0, 1024);

  if (isoptions(p)) {
    poptions_t o = CAST(poptions_t, p);
    if (MODE_ISANY(o->action, OPTPROGRAM_DEMANGLE)) {
      int n = sprintf(name0, "%s", name);
      if (n > 21) {
        sprintf(name0 + 16, "[...]");
      }

      return name0;
    }
  }

  return name;
}

