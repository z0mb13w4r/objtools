#include "opcode-engine-got.h"

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

static uint64_t execute_addr(handle_t p, const uchar_t v0, const uchar_t v1, const uchar_t v2, const uchar_t v3) {
  return v0 | (v1 << 8) | (v2 << 16) | (v3 << 24);
}

static void execute_section(handle_t p, handle_t s, handle_t q) {
  printf("%s %ld %lx %lx\n", ocget_name(s), ocget_size(s), ocget_vmaddress(s), ocget_lmaddress(s));

  puchar_t pp = ocget_rawdata(s);
  if (pp) {
    uint64_t prev_vaddr = 0;
    uint64_t curr_vaddr = ocget_vmaddress(s);
    for (uint64_t i = 0; i < ocget_size(s); ) {
      if (0xff == pp[i + 0] && 0x35 == pp[i + 1]) {
        printf(" pushq %02x %02x %08lx\n", pp[i + 0], pp[i + 1], execute_addr(p, pp[i + 2], pp[i + 3], pp[i + 4], pp[i + 5]));
        i += 6;
      } else {
        printf(" %02x", pp[i]);
        i += 1;
      }
    }
  }

  printf("\n");
}

static void callback_sections(handle_t p, handle_t shdr, unknown_t param) {
  const char* name = ocget_name(shdr);
  if (0 == xstrcmp(ocget_name(shdr), ".plt.got")) {
    execute_section(p, shdr, param);
  } else if (0 == xstrcmp(name, ".plt.sec")) {
    execute_section(p, shdr, param);
  } else if (0 == xstrcmp(name, ".plt")) {
    execute_section(p, shdr, param);
  }
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

