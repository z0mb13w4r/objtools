#include "ocdwarf.h"
#include "options.h"
#include "opcode-engine.h"
#include "opcode-printf.h"

static const int MAXSIZE = 24;

static int ocdebugf_cvalue0(handle_t p, uint64_t cv) {
  if (isopcode(p)) {
    int n = 0;

    n += printf_text("CVALUE", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(cv, USE_FHEX64);
    n += printf_pick(oegetINSTRUCTIONNAMES(p), OCINSN_MASK(cv), USE_SPACE);
    n += printf_mask(oegetINSTRUCTIONFLAGS(p), OCFLAG_MASK(cv), USE_NONE);
    n += printf_eol();

    return n;
  }

  return ECODE_HANDLE;
}

static int ocdebugf_cvalue1(handle_t p, uint64_t cv) {
  if (isopcode(p)) {
    int n = 0;

    n += printf_text("CVALUE", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(cv, USE_FHEX64);

    if (MODE_ISSET(cv, OPOPERAND_REGISTER0)) {
      n += printf_text("REGISTER0", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_IVALUE0)) {
      n += printf_text("IVALUE0", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_UVALUE0)) {
      n += printf_text("UVALUE0", USE_LT | USE_SPACE);
    }

    if (MODE_ISSET(cv, OPOPERAND_REGISTER1)) {
      n += printf_text("REGISTER1", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_IVALUE1)) {
      n += printf_text("IVALUE1", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_UVALUE1)) {
      n += printf_text("UVALUE1", USE_LT | USE_SPACE);
    }

    if (MODE_ISSET(cv, OPOPERAND_REGISTER2)) {
      n += printf_text("REGISTER2", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_IVALUE2)) {
      n += printf_text("IVALUE2", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_UVALUE2)) {
      n += printf_text("UVALUE2", USE_LT | USE_SPACE);
    }

    if (MODE_ISSET(cv, OPOPERAND_REGISTER3)) {
      n += printf_text("REGISTER3", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_IVALUE3)) {
      n += printf_text("IVALUE3", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_UVALUE3)) {
      n += printf_text("UVALUE3", USE_LT | USE_SPACE);
    }

    if (MODE_ISSET(cv, OPOPERAND_REGISTER4)) {
      n += printf_text("REGISTER4", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_IVALUE4)) {
      n += printf_text("IVALUE4", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_UVALUE4)) {
      n += printf_text("UVALUE4", USE_LT | USE_SPACE);
    }

    if (MODE_ISSET(cv, OPOPERAND_REGISTER5)) {
      n += printf_text("REGISTER5", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_IVALUE5)) {
      n += printf_text("IVALUE5", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_UVALUE5)) {
      n += printf_text("UVALUE5", USE_LT | USE_SPACE);
    }

    if (MODE_ISSET(cv, OPOPERAND_REGISTER6)) {
      n += printf_text("REGISTER6", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_IVALUE6)) {
      n += printf_text("IVALUE6", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_UVALUE6)) {
      n += printf_text("UVALUE6", USE_LT | USE_SPACE);
    }

    if (MODE_ISSET(cv, OPOPERAND_REGISTER7)) {
      n += printf_text("REGISTER7", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_IVALUE7)) {
      n += printf_text("IVALUE7", USE_LT | USE_SPACE);
    } else if (MODE_ISSET(cv, OCOPERAND_UVALUE7)) {
      n += printf_text("UVALUE7", USE_LT | USE_SPACE);
    }

    if (0 == cv) {
      n += printf_text("not set", USE_LT | USE_SPACE | USE_TB);
    }

    n += printf_mask(oeSEGMENTFLAGS_x86_64, MODE_HIDE16(cv), USE_NONE);
    n += printf_eol();

    return n;
  }

  return ECODE_HANDLE;
}

static int ocdebugf_nvalueX(handle_t p, const uint64_t cv, const uint64_t nv, const char *sv, const char *id, const uint64_t mask) {
  if (isopcode(p)) {
    int n = 0;

    if (MODE_ISSET(cv, MODE_ISANY(mask, OPOPERAND_REGISTERMASK))) {
      n += printf_yoke("REGISTER", id, USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_pick(oegetREGISTERNAMES(p), nv, USE_SPACE);
      n += printf_mask(oegetREGISTERFLAGS(p), MODE_HIDE8(nv), USE_NONE);
      n += printf_eol();
    } else if (MODE_ISSET(cv, MODE_ISANY(mask, OCOPERAND_IVALUEMASK))) {
      n += printf_yoke("IVALUE", id, USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(nv, USE_DEC);
      n += printf_eol();
    } else if (MODE_ISSET(cv, MODE_ISANY(mask, OCOPERAND_UVALUEMASK))) {
      n += printf_yoke("UVALUE", id, USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(nv, USE_FHEX64);
      n += printf_eol();
    } else if (sv[0]) {
      n += printf_yoke("UNKNOWN", id, USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_text(sv, USE_LT | USE_SPACE);
      n += printf_eol();
    }

    return n;
  }

  return ECODE_HANDLE;
}

static int ocdebugf_nvalueZ(handle_t p, unknown_t o, const char *name) {
  if (o && name) {
    int n = 0;
    pocoperand_t o0 = CAST(pocoperand_t, o);

    n += printf_text(name, USE_LT | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_text(o0->data, USE_LT | USE_SPACE | USE_EOL);
    n += ocdebugf_cvalue1(p, o0->cvalue);
    n += ocdebugf_nvalueX(p, o0->cvalue, o0->uvalue0, o0->svalue0, "0", OPOPERAND_REGISTER0);
    n += ocdebugf_nvalueX(p, o0->cvalue, o0->uvalue1, o0->svalue1, "1", OPOPERAND_REGISTER1);
    n += ocdebugf_nvalueX(p, o0->cvalue, o0->uvalue2, o0->svalue2, "2", OPOPERAND_REGISTER2);
    n += ocdebugf_nvalueX(p, o0->cvalue, o0->uvalue3, o0->svalue3, "3", OPOPERAND_REGISTER3);
    n += ocdebugf_nvalueX(p, o0->cvalue, o0->uvalue4, o0->svalue4, "4", OPOPERAND_REGISTER4);
    n += ocdebugf_nvalueX(p, o0->cvalue, o0->uvalue5, o0->svalue5, "5", OPOPERAND_REGISTER5);
    n += ocdebugf_nvalueX(p, o0->cvalue, o0->uvalue6, o0->svalue6, "6", OPOPERAND_REGISTER6);
    n += ocdebugf_nvalueX(p, o0->cvalue, o0->uvalue7, o0->svalue7, "7", OPOPERAND_REGISTER7);

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
    pocoperand_t o2 = oeget(q, OECODE_OPERAND3);
    pocoperand_t o3 = oeget(q, OECODE_OPERAND4);
    pocmnemonic_t m0 = oeget(q, OECODE_MNEMONIC);

    n += printf_eol();
    n += printf_mark('+', 100, USE_EOL);

    n += printf_text("VADDR", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
    n += opcode_printf_FADDR(p, q0->vaddr, USE_EOL);
    if (0 != q0->comment[0]) {
      n += printf_text("COMMENT", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_text(q0->comment, USE_LT | USE_SPACE | USE_EOL);
    }
    if (m0) {
      n += printf_text("MNEMONIC", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_text(m0->data, USE_LT | USE_SPACE | USE_EOL);
      n += ocdebugf_cvalue0(p, m0->cvalue);
      if (0 != m0->uvalue) {
        n += printf_text("UVALUE", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        n += opcode_printf_FADDR(p, m0->uvalue, USE_EOL);
      }
    }
    if (o0) {
      n += ocdebugf_nvalueZ(p, o0, "OPERAND1");
    }
    if (o1) {
      n += ocdebugf_nvalueZ(p, o1, "OPERAND2");
    }
    if (o2) {
      n += ocdebugf_nvalueZ(p, o2, "OPERAND3");
    }
    if (o3) {
      n += ocdebugf_nvalueZ(p, o3, "OPERAND4");
    }
    n += printf_mark('+', 100, USE_EOL);

    return n;
  }

  return ECODE_HANDLE;
}

int opcode_printf_DEC(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDISASSEMBLE_ENHANCED)) {
      return printf_nice(v, USE_DEC2 | mode);
    }
  }

  return printf_nice(v, USE_DEC | mode);
}

int opcode_printf_FHEX(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDISASSEMBLE_ENHANCED)) {
      return printf_nice(v, USE_FHEX32 | mode);
    }
  }

  return printf_nice(v, USE_FHEX | mode);
}

int opcode_printf_LHEX(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDISASSEMBLE_ENHANCED)) {
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

int opcode_printf_pick(handle_t p, const pconvert_t z, const pick_t x, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDISASSEMBLE_VERBOSE)) {
      n += printf_nice(x, USE_FHEX16);
    } else if (MODE_ISANY(oc->ocdump, OPTDISASSEMBLE_ENHANCED)) {
      n += printf_nice(x, USE_FHEX8);
    }

    n += printf_pick(z, x, USE_SPACE | (mode & ~USE_SPECIAL));
  }

  return n;
}

int opcode_printf_prefix(handle_t p, const uint64_t vaddr) {
  if (isopcode(p)) {
    int n = 0;
    char *name = NULL;
    uint64_t offset = 0;

    ocget_func(p, vaddr, &name, &offset);
    if (name && name[0]) {
      if (0 != offset) {
        n += printf_text(name, USE_LT | USE_SPACE | USE_TBLT);
        n += printf_text("+", USE_LT);
        n += printf_nice(offset, USE_FHEX | USE_TBRT | USE_NOSPACE);
      } else {
        n += printf_text(name, USE_LT | USE_SPACE | USE_TB);
      }
    }

    return n;
  }

  return ECODE_HANDLE;
}

int opcode_printf_source(handle_t p, const uint64_t vaddr) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    int n = 0;
    char *name = NULL;
    char *source = NULL;
    char *sourcecode = NULL;
    uint64_t curr_nline = 0;
    uint64_t curr_discriminator = 0;

    bool_t issp = ocget_symbol(p, vaddr, &name, &curr_nline, NULL, &curr_discriminator, &source, &sourcecode, NULL, NULL, NULL);

    uint32_t curr_name = xstrcrc32(name);
    uint32_t curr_source = xstrcrc32(source);
    uint32_t curr_sourcecode = xstrcrc32(sourcecode);

    bool_t isok = oc->prev_nline != curr_nline || oc->prev_discriminator != curr_discriminator
              || (source && curr_source != oc->prev_source)
              || (name && curr_name != oc->prev_name);

    if (isok && name && name[0] && (curr_name != oc->prev_name)) {
      if (MODE_ISNOT(oc->action, OPTPROGRAM_NO_ADDRESSES)) {
        n += opcode_printf_LADDR(p, vaddr, USE_NONE);
      }
      n += printf_text(name, USE_LT | USE_SPACE | USE_TB | USE_COLON | USE_EOL);

      if (MODE_ISANY(oc->action, OPTPROGRAM_LINE_NUMBERS) && issp) {
        n += printf_yoke(name, "()", USE_LT | USE_COLON | USE_EOL);
      }

      oc->prev_name = curr_name;
    }

    if (MODE_ISANY(oc->action, OPTPROGRAM_LINE_NUMBERS)) {
      if (isok && source) {
        n += printf_text(source, USE_LT | USE_COLON);
        n += printf_nice(curr_nline, USE_DEC | USE_NOSPACE);
        if (0 != curr_discriminator) {
          n += printf_nice(curr_discriminator, USE_DISCRIMINATOR);
        }
        n += printf_eol();

        if (sourcecode && (curr_sourcecode != oc->prev_sourcecode) && MODE_ISANY(oc->action, OPTPROGRAM_SOURCE_CODE)) {
          n += printf_text(sourcecode, USE_LT);
          n += printf_eol();
        }

        oc->prev_nline = curr_nline;
        oc->prev_discriminator = curr_discriminator;
        oc->prev_sourcecode = curr_sourcecode;
        oc->prev_source = curr_source;
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
    uint64_t offset = 0;

    pocexamine_t oe = oecreate(p, vaddr, mnemonic, operands);
    pocmnemonic_t m = oeget(oe, OECODE_MNEMONIC);
    pocoperand_t o1 = oeget(oe, OECODE_OPERAND1);
    pocoperand_t o2 = oeget(oe, OECODE_OPERAND2);
    pocoperand_t o3 = oeget(oe, OECODE_OPERAND3);
    popcode_t    oc = ocget(p, OPCODE_THIS);

    const bool_t isok = m && isused(oegetADDRLOOKUP(p), OCINSN_MASK(m->cvalue)) &&
      (
        (o1 && (MODE_ISLOCKED8(OCOPERAND_IVALUE0, o1->cvalue) || MODE_ISLOCKED8(OCOPERAND_UVALUE0, o1->cvalue))) ||
        (o2 && (MODE_ISLOCKED8(OCOPERAND_IVALUE0, o2->cvalue) || MODE_ISLOCKED8(OCOPERAND_UVALUE0, o2->cvalue))) ||
        (o3 && (MODE_ISLOCKED8(OCOPERAND_IVALUE0, o3->cvalue) || MODE_ISLOCKED8(OCOPERAND_UVALUE0, o3->cvalue)))
      );

    if (MODE_ISNOT(oc->action, OPTPROGRAM_NO_ADDRESSES) || !isok) {
      n += printf_text(mnemonic, USE_LT | USE_SPACE);
      if (operands) {
        n += printf_text(operands, USE_LT | USE_SPACE);
      }
    } else {
      n += printf_text(m->data, USE_LT | USE_SPACE);
    }

    if (m && o1 && (m->uvalue || isok)) {
      uint64_t uvalue = m->uvalue;
      if (o3 && (OCOPERAND_UVALUE0 == o3->cvalue)) {
        uvalue = o3->uvalue0;
      } else if (o2 && (OCOPERAND_UVALUE0 == o2->cvalue)) {
        uvalue = o2->uvalue0;
      } else if (o1 && (OCOPERAND_UVALUE0 == o1->cvalue)) {
        uvalue = o1->uvalue0;
      }
      ocget_symbol(p, uvalue, &name, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &offset);

      if (name && name[0]) {
        if (0 != offset) {
          n += printf_text(name, USE_LT | USE_SPACE | USE_TBLT);
          n += printf_text("+", USE_LT);
          n += printf_nice(offset, USE_FHEX | USE_TBRT | USE_NOSPACE);
        } else {
          n += printf_text(name, USE_LT | USE_SPACE | USE_TB);
        }
      }
    }
#ifndef OPCODE_EXAMINE_MNEMONIC
    if (MODE_ISANY(oc->ocdump, OPTDISASSEMBLE_VERBOSE)) {
#endif
      n += ocdebugf(p, oe);
#ifndef OPCODE_EXAMINE_MNEMONIC
    }
#endif
    oefree(oe);

    return n;
  }

  return ECODE_HANDLE;
}

