#include "ocdwarf.h"
#include "options.h"
#include "opcode-printf.h"
#include "opcode-examine.h"

static const int MAXSIZE = 24;

static int ocdebugf_cvalue(handle_t p, uint64_t cv) {
  if (isopcode(p)) {
    int n = 0;

    n += printf_text("CVALUE", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(cv, USE_FHEX64);

    if (MODE_ISLOCKED8(OCOPERAND_IVALUE, cv)) {
      n += printf_text("IVALUE", USE_LT | USE_SPACE);
    } else if (MODE_ISLOCKED8(OCOPERAND_UVALUE, cv)) {
      n += printf_text("UVALUE", USE_LT | USE_SPACE);
    } else if (MODE_ISLOCKED8(OPOPERAND_REGISTER, cv)) {
      n += printf_text("REGISTER", USE_LT | USE_SPACE);
    } else {
      n += printf_nice(MODE_MASK8(cv), USE_UNKNOWN);
    }

    if (MODE_ISANY(cv, OPOPERAND_SEGMENT)) {
      n += printf_text("| SEGMENT", USE_LT | USE_SPACE);
    }
    if (MODE_ISANY(cv, OCOPERAND_ABSOLUTE)) {
      n += printf_text("| ABSOLUTE", USE_LT | USE_SPACE);
    }
    if (MODE_ISANY(cv, OPSEGMENT_CS)) {
      n += printf_text("| CS", USE_LT | USE_SPACE);
    }
    if (MODE_ISANY(cv, OPSEGMENT_DS)) {
      n += printf_text("| DS", USE_LT | USE_SPACE);
    }
    if (MODE_ISANY(cv, OPSEGMENT_SS)) {
      n += printf_text("| SS", USE_LT | USE_SPACE);
    }
    if (MODE_ISANY(cv, OPSEGMENT_ES)) {
      n += printf_text("| ES", USE_LT | USE_SPACE);
    }
    if (MODE_ISANY(cv, OPSEGMENT_GS)) {
      n += printf_text("| GS", USE_LT | USE_SPACE);
    }
    if (MODE_ISANY(cv, OPSEGMENT_FS)) {
      n += printf_text("| FS", USE_LT | USE_SPACE);
    }

    n += printf_eol();

    return n;
  }

  return ECODE_HANDLE;
}

static int ocdebugf_nvalue(handle_t p, const uint64_t cv, const uint64_t nv) {
  if (isopcode(p)) {
    int n = 0;

    if (MODE_ISLOCKED8(OCOPERAND_IVALUE, cv)) {
      n += printf_text("IVALUE", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
    } else if (MODE_ISLOCKED8(OCOPERAND_UVALUE, cv)) {
      n += printf_text("UVALUE", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
    } else if (MODE_ISLOCKED8(OPOPERAND_REGISTER, cv)) {
      n += printf_text("REGISTER", USE_LT | USE_COLON | SET_PAD(MAXSIZE));

      if (MODE_ISANY(nv, OCREGISTER_GENERAL)) {
        n += printf_text("| GENERAL", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_INSTRUCTIONPTR)) {
        n += printf_text("| INSTRUCTION PTR", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_BASEPTR)) {
        n += printf_text("| BASE PTR", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_STACKPTR)) {
        n += printf_text("| STACK PTR", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_DSTINDEX)) {
        n += printf_text("| DST INDEX", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(cv, OCREGISTER_SRCINDEX)) {
        n += printf_text("| SRC INDEX", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_DATA)) {
        n += printf_text("| DATA", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_COUNTER)) {
        n += printf_text("| COUNTER", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(cv, OCREGISTER_BASE)) {
        n += printf_text("| BASE", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_ACCUMULATOR)) {
        n += printf_text("| ACCUMULATOR", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_8BITLO)) {
        n += printf_text("| 8 BIT LO", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_8BITHI)) {
        n += printf_text("| 8 BIT HI", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_16BIT)) {
        n += printf_text("| 16 BIT", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_32BIT)) {
        n += printf_text("| 32 BIT", USE_LT | USE_SPACE);
      }
      if (MODE_ISANY(nv, OCREGISTER_64BIT)) {
        n += printf_text("| 64 BIT", USE_LT | USE_SPACE);
      }
    } else {
      n += printf_text("UNKNOWN", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
    }

    if (!MODE_ISLOCKED8(OPOPERAND_REGISTER, cv)) {
      n += printf_nice(nv, USE_FHEX64);
    }
    n += printf_eol();

    return n;
  }

  return ECODE_HANDLE;
}

static int ocdebugf(handle_t p, handle_t q) {
  if (isopcode(p) && isocexamine(q)) {
    int n = 0;

    pocexamine_t q0 = oeget(q, OECODE_THIS);
    pocoperand_t o0 = oeget(q, OECODE_OPERAND1);
    pocoperand_t o1 = oeget(q, OECODE_OPERAND2);
    pocmnemonic_t m0 = oeget(q, OECODE_MNEMONIC);

    n += printf_eol();

    n += printf_text("VADDR", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
    n += opcode_printf_FADDR(p, q0->vaddr, USE_EOL);
    if (0 != q0->comment[0]) {
      n += printf_text("COMMENT", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_text(q0->comment, USE_LT | USE_EOL);
    }
    if (m0) {
      n += printf_text("MNEMONIC", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_text(m0->data, USE_LT | USE_EOL);
    }
    if (o0) {
      n += printf_text("OPERAND1", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_text(o0->data, USE_LT | USE_EOL);
      n += ocdebugf_cvalue(p, o0->cvalue);
      n += ocdebugf_nvalue(p, o0->cvalue, o0->uvalue);
    }
    if (o1) {
      n += printf_text("OPERAND2", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_text(o1->data, USE_LT | USE_EOL);
      n += ocdebugf_cvalue(p, o1->cvalue);
      n += ocdebugf_nvalue(p, o1->cvalue, o1->uvalue);
    }

    return n;
  }

  return ECODE_HANDLE;
}

int opcode_printf_DEC(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      return printf_nice(v, USE_DEC2 | mode);
    }
  }

  return printf_nice(v, USE_DEC | mode);
}

int opcode_printf_FHEX(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      return printf_nice(v, USE_FHEX32 | mode);
    }
  }

  return printf_nice(v, USE_FHEX | mode);
}

int opcode_printf_LHEX(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      return printf_nice(v, USE_LHEX32 | mode);
    }
  }

  return printf_nice(v, USE_LHEX | mode);
}

int opcode_printf_FADDR(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    if (ocis32(p)) {
      return printf_nice(v, USE_FHEX32 | mode);
    } else if (ocis64(p)) {
      return printf_nice(v, USE_FHEX64 | mode);
    }
  }

  return printf_nice(v, USE_FHEX32 | mode);
}

int opcode_printf_LADDR(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    if (ocis32(p)) {
      return printf_nice(v, USE_LHEX32 | mode);
    } else if (ocis64(p)) {
      return printf_nice(v, USE_LHEX64 | mode);
    }
  }

  return printf_nice(v, USE_LHEX32 | mode);
}

int opcode_printf_pluck(handle_t p, const pconvert_t z, const pick_t x, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->action, OPTPROGRAM_VERBOSE)) {
      n += printf_nice(x, USE_FHEX16);
    } else if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n += printf_nice(x, USE_FHEX8);
    }

    n += printf_pick(z, x, USE_SPACE | (mode & ~USE_SPECIAL));
  }

  return n;
}

int opcode_printf_source(handle_t p, const uint64_t vaddr) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    int n = 0;
    char *name = NULL;
    char *source = NULL;
    Dwarf_Unsigned nline = 0;
    Dwarf_Unsigned discriminator = 0;

    ocdwarf_spget(p, vaddr, &name, &nline, NULL, &discriminator, &source, NULL, NULL, NULL, NULL);

    bool_t isok = oc->prev_nline != nline || oc->prev_discriminator != discriminator;

    if (isok && 0 != name) {
      n += opcode_printf_LADDR(p, vaddr, USE_NONE);
      n += printf_text(name, USE_LT | USE_SPACE | USE_TB | USE_COLON | USE_EOL);

      if (MODE_ISANY(oc->action, OPTPROGRAM_LINE_NUMBERS)) {
        n += printf_yoke(name, "()", USE_LT | USE_COLON | USE_EOL);
      }

      ocdwarf_dealloc(p, name, DW_DLA_STRING);
    }

    if (MODE_ISANY(oc->action, OPTPROGRAM_LINE_NUMBERS)) {
      if (isok && 0 != source) {
        n += printf_text(source, USE_LT | USE_COLON);
        n += printf_nice(nline, USE_DEC | USE_NOSPACE);
        if (0 != discriminator) {
          n += printf_nice(discriminator, USE_DISCRIMINATOR);
        }
        n += printf_eol();

        oc->prev_nline = nline;
        oc->prev_discriminator = discriminator;

        ocdwarf_dealloc(p, source, DW_DLA_STRING);
      }
    }

    return n;
  }

  return ECODE_HANDLE;
}

int opcode_printf_detail(handle_t p, const uint64_t vaddr, unknown_t mnemonic, unknown_t operands) {
  if (isopcode(p)) {
    int n = 0;
    char *name = NULL;
    Dwarf_Off offset = 0;

    pocexamine_t oe = oecreate(p, vaddr, mnemonic, operands);

    if (oe && oe->op1) {
      ocdwarf_spget(p, oe->op1->uvalue, &name, NULL, NULL, NULL, NULL, NULL, NULL, &offset, NULL);

      if (0 != name) {
        if (0 != offset) {
          n += printf_text(name, USE_LT | USE_SPACE | USE_TBLT);
          n += printf_text("+", USE_LT);
          n += printf_nice(offset, USE_FHEX | USE_TBRT | USE_NOSPACE);
        } else {
          n += printf_text(name, USE_LT | USE_SPACE | USE_TB);
        }

        ocdwarf_dealloc(p, name, DW_DLA_STRING);
      }
    }

    n += ocdebugf(p, oe);

    oefree(oe);

    return n;
  }

  return ECODE_HANDLE;
}

