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
    return 0;
  }

  return -1;
}

int capstone_raw(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr) {
  int n = 0;
  if (data && isopcode(p) && isopshdrNN(s)) {
    popcode_t oc = CAST(popcode_t, p);

    cs_insn *insn = NULL;
    size_t count = cs_disasm(oc->cs, data, size, vaddr, 0, &insn);
    if (count > 0) {
      for (size_t j = 0; j < count; ++j) {
        int n1 = 0;
        n1 += printf_nice(insn[j].address, USE_LHEX32 | USE_COLON);
        n1 += printf_sore(insn[j].bytes, insn[j].size, USE_HEX | USE_SPACE);
        n1 += printf_pack(42 - n1);
        n1 += printf_text(insn[j].mnemonic, USE_LT | USE_SPACE);
        n1 += printf_text(insn[j].op_str, USE_LT | USE_SPACE | USE_EOL);

        n += n1;
      }

      cs_free(insn, count);
    } else {
      printf_e("Failed to disassemble given code!");
    }
  }

  return n;
}

int capstone_run(handle_t p, handle_t s) {
  int n = 0;
  if (isopcode(p) && isopshdr(s)) {
    const size_t sz = ocget_size(s);
    unknown_t    p0 = mallocx(sz);

    if (p0) {
      if (bfd_get_section_contents(ocget(p, OPCODE_BFD), ocget(s, MODE_OCSHDR), p0, 0, sz)) {
        n += capstone_raw(p, s, p0, sz, ocget_vmaddress(s));
      }

      free(p0);
    }
  } else if (isopcode(p) && isopshdrNN(s)) {
    const size_t sz = ocget_size(s);
    if (0 != sz && ocget_type(s) != SHT_NOBITS) {
      n += capstone_raw(p, s, getp(ocget(p, OPCODE_RAWDATA), ocget_offset(s), sz), sz, ocget_vmaddress(s));
    }
  }

  return n;
}

