#include "opcode-engine-got.h"

static handle_t execute_new(handle_t p, const uint64_t vaddr, const char* name) {
  if (isocengine(p)) {
    pocgroups_t g0 = oegetbyaddr(p, vaddr, OPENGINE_GROUP);
    if (g0) {
      if (NULL == g0->symbol) {
        g0->symbol = osmalloc(vaddr);
      }
      g0->symbol->name = xstrdup(name);
      g0->symbol->role |= OPSYMBOL_NAME;
    }
  }

  return p;
}

handle_t opcode_create(handle_t p, handle_t q) {
  if (isopcode(p) && isocengine(q)) {
    execute_new(q, 0x10b0, "__cxa_finalize@plt");
    execute_new(q, 0x10c0, "strcpy@plt");
    execute_new(q, 0x10d0, "puts@plt");
    execute_new(q, 0x10e0, "__stack_chk_fail@plt");
    execute_new(q, 0x10f0, "read@plt");
    execute_new(q, 0x1100, "strcmp@plt");
    execute_new(q, 0x1110, "malloc@plt");
    execute_new(q, 0x1120, "__printf_chk@plt");
    execute_new(q, 0x1130, "__ctype_b_loc@plt");
    return q;
  }

  return NULL;
}

