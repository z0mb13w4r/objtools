#include "opcode.h"
#include "printf.h"
#include "ocdwarf.h"
#include "options.h"
#include "opcode-printf.h"
#include "opcode-capstone.h"

static int get_csarch(handle_t p, handle_t o) {
  if (isoptions(o)) {
    poptions_t op = CAST(poptions_t, o);
    if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_X86_64 | OPTDISASSEMBLE_I386 | OPTDISASSEMBLE_I8086)) {
      return CS_ARCH_X86;
    }
  }

  return CS_ARCH_X86; // TBD
}

static int get_csmode(handle_t p, handle_t o) {
  if (isoptions(o)) {
    poptions_t op = CAST(poptions_t, o);
    if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_X86_64))      return CS_MODE_64;
    else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_I386))   return CS_MODE_32;
    else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_I8086))  return CS_MODE_16;
  }

  const uint64_t size = ocget_archsize(p);
  if (16 == size) return CS_MODE_16;
  else if (32 == size) return CS_MODE_32;

  return CS_MODE_64;
}

int capstone_open(handle_t p, handle_t o) {
  if (isopcode(p) && isoptions(o)) {
    poptions_t op = CAST(poptions_t, o);
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (CS_ERR_OK == cs_open(get_csarch(p, o), get_csmode(p, o), &oc->cs)) {
      if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_ATT_MNEMONIC)) {
        cs_option(oc->cs, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);
      } else if (MODE_ISSET(op->ocdump, OPTDISASSEMBLE_INTEL_MNEMONIC)) {
        cs_option(oc->cs, CS_OPT_SYNTAX, CS_OPT_SYNTAX_INTEL);
      } else {
        cs_option(oc->cs, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);
      }

//    cs_option(oc->cs, CS_OPT_DETAIL, CS_OPT_ON);
      return 0;
    }
  }

  return -1;
}

int capstone_close(handle_t p) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    cs_close(&oc->cs);
    return 0;
  }

  return -1;
}

int capstone_raw(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr) {
  int n = 0;
  if (data && isopcode(p) && ismodeNXXN(s, MODE_OCSHDRWRAP)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    Dwarf_Unsigned prev_nline = 0;
    Dwarf_Unsigned prev_discriminator = 0;

    cs_insn *insn = NULL;
    size_t count = cs_disasm(oc->cs, data, size, vaddr, 0, &insn);
    if (count > 0) {
      bool_t bskip = FALSE;
      uchar_t iskip = 0;
      for (size_t i = 0; i < count; ++i) {
        if (ocuse_vaddr(p, insn[i].address)) {
          int n1 = 0;
          int n2 = 0;
          if (bskip && iskip != insn[i].bytes[0]) {
            n1 += printf_text(">>>>>>>>", USE_LT | USE_SPACE | USE_COLON);
            n1 += printf_eol();
            bskip = FALSE;
          }
          if (!bskip) {
            char *name = NULL;
            char *source = NULL;
            Dwarf_Unsigned nline = 0;
            Dwarf_Unsigned discriminator = 0;

            n2 += ocdwarf_spget(p, insn[i].address, &name, &nline, NULL, &discriminator, &source, NULL, NULL, NULL);

            bool_t isok = prev_nline != nline || prev_discriminator != discriminator;

            if (isok && 0 != name) {
              n2 += opcode_printf_LADDR(p, insn[i].address, USE_NONE);
              n2 += printf_text(name, USE_LT | USE_SPACE | USE_TB | USE_COLON | USE_EOL);

              n2 += printf_yoke(name, "()", USE_LT | USE_COLON | USE_EOL);
            }
            if (isok && 0 != source) {
              n2 += printf_text(source, USE_LT | USE_COLON);
              n2 += printf_nice(nline, USE_DEC | USE_NOSPACE);
              if (0 != discriminator) {
                n2 += printf_nice(discriminator, USE_DISCRIMINATOR);
              }
              n2 += printf_eol();

              prev_nline = nline;
              prev_discriminator = discriminator;
            }

            n2 += opcode_printf_LHEX(p, insn[i].address, USE_COLON);
            n2 += printf_sore(insn[i].bytes, insn[i].size, USE_HEX | USE_SPACE);
            n2 += printf_pack(42 - n2);
            n2 += printf_text(insn[i].mnemonic, USE_LT | USE_SPACE);
            n2 += printf_text(insn[i].op_str, USE_LT | USE_SPACE);
            n2 += printf_eol();
          }
          if (0xcc == insn[i].bytes[0] || 0x90 == insn[i].bytes[0] || 0x00 == insn[i].bytes[0]) {
            iskip = insn[i].bytes[0];
            bskip = TRUE;
          }
          n += n1 + n2;
        }
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
    unknown_t    p0 = xmalloc(sz);

    if (p0) {
      if (bfd_get_section_contents(ocget(p, OPCODE_BFD), ocget(s, MODE_OCSHDR), p0, 0, sz)) {
        n += capstone_raw(p, s, p0, sz, ocget_vmaddress(s));
      }

      free(p0);
    }
  } else if (isopcode(p) && ismodeNXXN(s, MODE_OCSHDRWRAP)) {
    const size_t sz = ocget_size(s);
    if (0 != sz) {
      n += capstone_raw(p, s, getp(ocget(p, OPCODE_RAWDATA), ocget_offset(s), sz), sz, ocget_vmaddress(s));
    }
  }

  return n;
}

