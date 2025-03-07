#include "opcode.h"
#include "printf.h"
#include "opcode-capstone.h"

static int get_csarch(handle_t p) {
  return CS_ARCH_X86; // TBD
}

static int get_csmode(handle_t p) {
  const uint64_t size = ocget_archsize(p);
  if (16 == size) return CS_MODE_16;
  else if (32 == size) return CS_MODE_32;

  return CS_MODE_64;
}

int capstone_open(handle_t p, handle_t o) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    if (CS_ERR_OK == cs_open(get_csarch(p), get_csmode(p), &oc->cs)) {
//    cs_option(oc->cs, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT); // -M
//    cs_option(oc->cs, CS_OPT_SYNTAX, CS_OPT_SYNTAX_INTEL); // default
//    cs_option(oc->cs, CS_OPT_DETAIL, CS_OPT_ON);
      return 0;
    }
  }

  return -1;
}

int capstone_close(handle_t p) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    cs_close(&oc->cs);
  }

  return -1;
}

int capstone_run(handle_t p, handle_t s) {
  int r = -1;
  if (isopcode(p) && isopsection(s)) {
    popcode_t oc = CAST(popcode_t, p);

    size_t data_size = ocget_size(s);
    void*  data = mallocx(data_size);

    if (data) {
      if (bfd_get_section_contents(oc->items[OPCODE_BFD], ocget(s, MODE_OPSECTION), data, 0, data_size)) {

        cs_insn *insn = NULL;
        size_t count = cs_disasm(oc->cs, data, data_size, ocget_vmaddress(s), 0, &insn);
        if (count > 0) {
          for (size_t j = 0; j < count; ++j) {
            int n = 0;
            n += printf_nice(insn[j].address, USE_LHEX16 | USE_COLON);
            n += printf_data(insn[j].bytes, insn[j].size, 0, USE_HEX | USE_SPACE);
            n += printf_pack(40 - n);
            n += printf_text(insn[j].mnemonic, USE_LT | USE_SPACE);
            n += printf_text(insn[j].op_str, USE_LT | USE_SPACE | USE_EOL);
          }

          cs_free(insn, count);
          r = 0;
        } else {
          printf_e("Failed to disassemble given code!");
        }
      }

      free(data);
    }
  }

  return r;
}

