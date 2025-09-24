#include "opcode-engine-got.h"


#include "printf.h"

static void callback_sections(handle_t p, handle_t shdr, unknown_t param) {
  printf_text(ocget_name(shdr), USE_LT);
  printf_nice(ocget_size(shdr), USE_LHEX32);
  printf_nice(ocget_vmaddress(shdr), USE_LHEX32);
  printf_nice(ocget_lmaddress(shdr), USE_LHEX32);
  printf_nice(ocget_position(shdr), USE_LHEX32);

  uint64_t flags = ocget_flags(shdr);


  printf_eol();
}

static handle_t execute_new(handle_t p, const uint64_t vaddr, const char* name) {
  if (isoengine(p)) {
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
  if (isopcode(p) && isoengine(q)) {
    execute_new(q, 0x10b0, "__cxa_finalize@plt");
    execute_new(q, 0x10c0, "strcpy@plt");
    execute_new(q, 0x10d0, "puts@plt");
    execute_new(q, 0x10e0, "__stack_chk_fail@plt");
    execute_new(q, 0x10f0, "read@plt");
    execute_new(q, 0x1100, "strcmp@plt");
    execute_new(q, 0x1110, "malloc@plt");
    execute_new(q, 0x1120, "__printf_chk@plt");
    execute_new(q, 0x1130, "__ctype_b_loc@plt");

    ocdo_sections(p, callback_sections, q);
    return q;
  }

  return NULL;
}

