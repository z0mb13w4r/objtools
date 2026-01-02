#include "decode.h"
#include "options.h"
#include "ocdwarf-printf.h"

Dwarf_Unsigned nline = 0;
Dwarf_Unsigned ncolumn = 0;
Dwarf_Addr low_pc_addr = 0;

static const int MAXMERIT = 18;
static const int MAXGROUP = 24;

int ocdwarf_printf_me(handle_t p, const int x, const char *y, const char *z, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      n += printf_nice(x, USE_DEC2 | USE_TB | USE_NOSPACE);
      n += printf_text(y, USE_LT | USE_SPACE | USE_COLON | SET_PAD(23));
      n += printf_text(z, USE_LT | USE_SPACE | USE_DQ | mode);
    }
  }

  return n;
}

int ocdwarf_printf_pick(handle_t p, const pconvert_t z, const pick_t x, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      n += printf_nice(x, USE_FHEX16);
    } else if (MODE_ISANY(mode, USE_SPECIAL) && MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      n += printf_nice(x, USE_FHEX8);
    }

    n += printf_pick(z, x, USE_SPACE | (mode & ~USE_SPECIAL));
  }

  return n;
}

int ocdwarf_printf_picknull(handle_t p, const pconvert_t z, const pick_t x, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      n += printf_nice(x, USE_FHEX16);
    }

    n += printf_picknull(z, x, USE_SPACE | (mode & ~USE_SPECIAL));
  }

  return n;
}

int ocdwarf_printf_DEC(handle_t p, const uint64_t v, const imode_t mode) {
  const bool_t isset = GET_BRACKET(mode) ? TRUE : FALSE;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      return printf_nice(v, isset ? USE_DEC2 | mode : USE_DEC2 | USE_TB | mode);
    }
  }

  return printf_nice(v, isset ? USE_DEC | mode : USE_DEC | USE_TB | mode);
}

int ocdwarf_printf_HEX(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      return printf_nice(v, USE_FHEX32 | USE_TB | mode);
    }
  }

  return printf_nice(v, USE_LHEX | USE_TB | mode);
}

int ocdwarf_printf_ADDR(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      return printf_nice(v, USE_FHEX32 | mode);
    } else if (MODE_ISANY(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED)) {
      return printf_nice(v, USE_LHEX32 | mode);
    }
  }

  return printf_nice(v, USE_FHEX | mode);
}

int ocdwarf_printf_AT(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pick(p, ecDWAT, v, mode | SET_PAD(30));
}

int ocdwarf_printf_OP(handle_t p, const uint64_t v, const imode_t mode) {
  if (DW_OP_breg0 <= v && v <= DW_OP_breg31) {
    int n = 0;

    const imode_t modex = mode & ~(USE_BRACKETMASK | USE_POS0MASK | USE_POS1MASK);
    const imode_t mode0 = GET_POS0(mode) | make_spos(mode);
    const imode_t mode1 = GET_POS1(mode) | make_epos(mode);

    n += ocdwarf_printf_pick(p, ecDWOP, v, modex | mode0);
    n += ocdwarf_printf_REG(p, v - DW_OP_breg0, modex | mode1);
    return n;
  }

  return ocdwarf_printf_pick(p, ecDWOP, v, mode);
}

int ocdwarf_printf_ATE(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISNOT(oc->ocdump, OPTDWARF_ENHANCED)) {
      int n = 0;
      n += printf_nice(v, USE_DEC);
      n += ocdwarf_printf_pick(p, ecDWATELITE, v, USE_RB | mode);
      return n;
    }
  }

  return ocdwarf_printf_pick(p, ecDWATE, v, mode);
}

int ocdwarf_printf_CFA(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      return ocdwarf_printf_pick(p, ecDWCFA, v, mode);
    } else if (MODE_ISFIX(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED, OPTDWARF_ENHANCED)) {
      return ocdwarf_printf_picknull(p, ecDWCFALITE, v, mode);
    }
  }

  return ocdwarf_printf_pick(p, ecDWCFA, v, mode);
}

int ocdwarf_printf_TAG(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pick(p, ecDWTAG, v, mode);
}

int ocdwarf_printf_EXPR(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      return ocdwarf_printf_pick(p, ecDWEXPR, v, mode);
    } else if (MODE_ISSET(oc->ocdump, OPTDWARF_ENHANCED)) {
      return ocdwarf_printf_pick(p, ecDWEXPRLITE, v, mode);
    }
  }

  return ocdwarf_printf_pick(p, ecDWEXPR, v, mode);
}

int ocdwarf_printf_GNUM(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pick(p, ecDWGNUM, v, mode);
}

int ocdwarf_printf_FORM(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pick(p, ecDWFORM, v, mode | SET_PAD(22));
}

int ocdwarf_printf_LNS(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pick(p, ecDWLNS, v, mode);
}

int ocdwarf_printf_LANG(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pick(p, ecDWLANG, v, mode);
}

int ocdwarf_printf_MACRO(handle_t p, const uint64_t v, const imode_t mode) {
  if (0 == v && MODE_ISANY(USE_SPECIAL, mode)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    return MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED) ?
      printf_nice(v, USE_FHEX8) + printf_text("end-of-macros", USE_LT | USE_SPACE) : 0;
  }

  return ocdwarf_printf_pick(p, ecDWMACRO, v, mode);
}

int ocdwarf_printf_CHILDREN(handle_t p, const uint64_t v, const imode_t mode) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISNOT(oc->ocdump, OPTDWARF_ENHANCED)) {
      return ocdwarf_printf_pick(p, ecDWCHILDRENLITE, v, USE_SB | mode);
    }
  }

  return ocdwarf_printf_pick(p, ecDWCHILDREN, v, mode);
}

int ocdwarf_printf_SRCFILE(handle_t p, const uint32_t v, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    pdwarf_srcfiles_t sf = ocget(p, OPCODE_DWARF_SRCFILES);
    if (sf && IS_DLV_OK(sf->status) && (0 != sf->size) && ((v - 1) < sf->size)) {
      popcode_t oc = ocget(p, OPCODE_THIS);

      n += printf_nice(v, mode);
      if (MODE_ISNOT(oc->ocdump, OPTDWARF_ENHANCED)) {
        n += printf_text("filename", USE_LT | USE_SPACE | USE_COLON);
      }
      n += printf_text(sf->data[v - 1], USE_LT | USE_SPACE);
    }
  }

  return n;
}

int ocdwarf_printf_REG(handle_t p, const uint32_t v, const imode_t mode) {
  int n = 0;
  if (ocisELF(p)) {
    const uint64_t e = ocget_machine(p);
    if (EM_386 == e) {
      n += ocdwarf_printf_pick(p, ecREGISTERS_i386, v, mode);
    } else if (EM_X86_64 == e) {
      n += ocdwarf_printf_pick(p, ecREGISTERS_x86_64, v, mode);
    } else if (EM_AARCH64 == e) {
      n += ocdwarf_printf_pick(p, ecREGISTERS_ARM64, v, mode);
    }
  }

  return n;
}

int ocdwarf_printf_REGISTER(handle_t p, const uint32_t v, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    if (MODE_ISNOT(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED) || MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      n += printf_join("r", v, USE_DEC | USE_SPACE);
    }

    if (MODE_ISNOT(oc->ocdump, OPTDWARF_ENHANCED)) {
      n += ocdwarf_printf_REG(p, v, mode);
    }
  }

  return n;
}

int ocdwarf_printf_expression_block(handle_t p, Dwarf_Block *expression_block) {
  int n = 0;
  if (isopcode(p) && expression_block) {

  }

  return n;
}

static int ocdwarf_printf_fields_description_0(handle_t p, Dwarf_Small cfa_operation, const char* fields_description,
                     Dwarf_Unsigned u0, Dwarf_Unsigned u1, Dwarf_Unsigned u2, Dwarf_Signed s0, Dwarf_Signed s1,
                     Dwarf_Unsigned code_alignment_factor, Dwarf_Signed data_alignment_factor, Dwarf_Block *expression_block) {
  int n = 0;
  if (isopcode(p) && fields_description && expression_block) {
    n += printf_nice(fields_description[0], USE_UNKNOWN | USE_TB);
  }

  return n;
}

static int ocdwarf_printf_fields_description_b(handle_t p, Dwarf_Small cfa_operation, const char* fields_description,
                     Dwarf_Unsigned u0, Dwarf_Unsigned u1, Dwarf_Unsigned u2, Dwarf_Signed s0, Dwarf_Signed s1,
                     Dwarf_Unsigned code_alignment_factor, Dwarf_Signed data_alignment_factor, Dwarf_Block *expression_block) {
  int n = 0;
  if (isopcode(p) && fields_description && expression_block) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      printf_nice(fields_description[1], USE_CHARCTRL | USE_TB);
    }

    if (0 != fields_description[1]) {
      n += printf_text("expr block len", USE_LT | USE_SPACE);
      n += printf_nice(expression_block->bl_len, USE_DEC);
      n += printf_hurt(expression_block->bl_data, expression_block->bl_len, USE_HEX | USE_SPACE | USE_0x);
      if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
        n += ocdwarf_printf_expression_block(p, expression_block);
      }
    }
  }

  return n;
}

static int ocdwarf_printf_fields_description_r(handle_t p, Dwarf_Small cfa_operation, const char* fields_description,
                     Dwarf_Unsigned u0, Dwarf_Unsigned u1, Dwarf_Unsigned u2, Dwarf_Signed s0, Dwarf_Signed s1,
                     Dwarf_Unsigned code_alignment_factor, Dwarf_Signed data_alignment_factor, Dwarf_Block *expression_block) {
  int n = 0;
  if (isopcode(p) && fields_description && expression_block) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      printf_nice(fields_description[1], USE_CHARCTRL | USE_TB);
    }

    const bool_t  TRY_MODE  = MODE_ISNOT(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED) || MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED);
    const imode_t TRY_RB    = TRY_MODE ? USE_RB : USE_NONE;
    const imode_t TRY_SDEC8 = TRY_MODE ? USE_SDEC8 : USE_SDEC8P | USE_NOSPACE;

    if (0 == fields_description[1]) {
      n += ocdwarf_printf_REGISTER(p, u0, TRY_RB);
    } else if ('u' == fields_description[1]) {
      if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
        printf_nice(fields_description[2], USE_CHARCTRL | USE_TB);
      }

      n += ocdwarf_printf_REGISTER(p, u0, TRY_RB);
      if (0 == fields_description[2]) {
        if (MODE_ISNOT(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED | OPTDWARF_ENHANCED)) {
          n += ocdwarf_printf_pick(p, ecDWCFAJOIN, cfa_operation, USE_SPACE);
        }

        n += printf_nice(u1, TRY_SDEC8);
      } else if ('d' == fields_description[2]) {
        if (MODE_ISNOT(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED | OPTDWARF_ENHANCED)) {
          n += ocdwarf_printf_pick(p, ecDWCFAJOIN, cfa_operation, USE_SPACE);
          n += printf_nice(u1 * data_alignment_factor, USE_DEC | USE_NOSPACE);
        } else {
          n += printf_nice(u1 * data_alignment_factor, USE_DEC);
        }

        if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
          n += printf_tack(u1, USE_DEC, "*", data_alignment_factor, USE_DEC, USE_RB);
        }
      } else if ('a' == fields_description[2]) {
        n += printf_nice(u1, USE_DEC);
        n += printf_nice(u1, USE_DEC | USE_RBLT);
        n += printf_text(", addrspace", USE_LT);
        n += printf_nice(u2, USE_DEC | USE_RBRT);
      }
    } else if ('r' == fields_description[1]) {
      n += ocdwarf_printf_REGISTER(p, u0, TRY_RB);
      n += ocdwarf_printf_REGISTER(p, u1, TRY_RB);
    } else if ('s' == fields_description[1]) {
      if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
        printf_nice(fields_description[2], USE_CHARCTRL | USE_TB);
      }

      if ('d' == fields_description[2]) {
        if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
          printf_nice(fields_description[3], USE_CHARCTRL | USE_TB);
        }

        n += ocdwarf_printf_REGISTER(p, u0, TRY_RB);
        n += printf_nice(s1 * data_alignment_factor, USE_DEC);
        if ('a' == fields_description[3]) {
          n += printf_text(", addrspace", USE_LT);
          n += printf_nice(u2, USE_DEC);
        }
        if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
          n += printf_tack(s1, USE_DEC, "*", data_alignment_factor, USE_DEC, USE_RB);
        }
      }
    } else if ('b' == fields_description[1]) {
      n += ocdwarf_printf_REGISTER(p, u0, TRY_RB);
      n += printf_text("expr block len", USE_LT | USE_SPACE);
      n += printf_nice(expression_block->bl_len, USE_DEC);
      n += printf_hurt(expression_block->bl_data, expression_block->bl_len, USE_HEX | USE_SPACE | USE_0x);
      if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
        n += ocdwarf_printf_expression_block(p, expression_block);
      }
    }
  }

  return n;
}

static int ocdwarf_printf_fields_description_s(handle_t p, Dwarf_Small cfa_operation, const char* fields_description,
                     Dwarf_Unsigned u0, Dwarf_Unsigned u1, Dwarf_Unsigned u2, Dwarf_Signed s0, Dwarf_Signed s1,
                     Dwarf_Unsigned code_alignment_factor, Dwarf_Signed data_alignment_factor, Dwarf_Block *expression_block) {
  int n = 0;
  if (isopcode(p) && fields_description && expression_block) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      printf_nice(fields_description[1], USE_CHARCTRL | USE_TB);
    }

    if ('d' == fields_description[1]) {
      n += printf_nice(s0 * data_alignment_factor, USE_DEC);
      if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
        n += printf_tack(s0, USE_DEC, "*", data_alignment_factor, USE_DEC, USE_RB);
      }
    }
  }

  return n;
}

static int ocdwarf_printf_fields_description_u(handle_t p, Dwarf_Small cfa_operation, const char* fields_description,
                     Dwarf_Unsigned u0, Dwarf_Unsigned u1, Dwarf_Unsigned u2, Dwarf_Signed s0, Dwarf_Signed s1,
                     Dwarf_Unsigned code_alignment_factor, Dwarf_Signed data_alignment_factor, Dwarf_Block *expression_block) {
  int n = 0;
  if (isopcode(p) && fields_description && expression_block) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      printf_nice(fields_description[1], USE_CHARCTRL | USE_TB);
    }

    if (0 == fields_description[1]) {
      n += printf_nice(u0, USE_DEC);
    } else if ('c' == fields_description[1]) {
      n += printf_nice(u0 * code_alignment_factor, USE_DEC);
      if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
        n += printf_tack(u0, USE_DEC, "*", code_alignment_factor, USE_DEC, USE_RB);
      }
    }
  }

  return n;
}

int ocdwarf_printf_fields_description(handle_t p, Dwarf_Small cfa_operation, const char* fields_description,
                     Dwarf_Unsigned u0, Dwarf_Unsigned u1, Dwarf_Unsigned u2, Dwarf_Signed s0, Dwarf_Signed s1,
                     Dwarf_Unsigned code_alignment_factor, Dwarf_Signed data_alignment_factor, Dwarf_Block *expression_block) {
  int n = 0;
  if (isopcode(p) && fields_description) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      printf_nice(fields_description[0], USE_CHARCTRL | USE_TB);
    }

    if ('u' == fields_description[0]) {
      n += ocdwarf_printf_fields_description_u(p, cfa_operation, fields_description, u0, u1, u2, s0, s1,
                     code_alignment_factor, data_alignment_factor, expression_block);
    } else if ('r' == fields_description[0]) {
      n += ocdwarf_printf_fields_description_r(p, cfa_operation, fields_description, u0, u1, u2, s0, s1,
                     code_alignment_factor, data_alignment_factor, expression_block);
    } else if ('s' == fields_description[0]) {
      n += ocdwarf_printf_fields_description_s(p, cfa_operation, fields_description, u0, u1, u2, s0, s1,
                     code_alignment_factor, data_alignment_factor, expression_block);
    } else if ('b' == fields_description[0]) {
      n += ocdwarf_printf_fields_description_b(p, cfa_operation, fields_description, u0, u1, u2, s0, s1,
                     code_alignment_factor, data_alignment_factor, expression_block);
    } else if (0 != fields_description[0]) {
      n += ocdwarf_printf_fields_description_0(p, cfa_operation, fields_description, u0, u1, u2, s0, s1,
                     code_alignment_factor, data_alignment_factor, expression_block);
    }
  }

  return n;
}

int ocdwarf_printf_worth(handle_t p, Dwarf_Die die, Dwarf_Attribute attr, Dwarf_Signed index, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    Dwarf_Half nattr = 0;
    x = dwarf_whatattr(attr, &nattr, e);
    if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
      printf_e("dwarf_whatattr failed! errcode %d", x);
#endif
      return OCDWARF_ERRCODE(x, n);
    }

    n += printf_pack(MAXMERIT);

    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      n += ocdwarf_printf_DEC(p, index, USE_TAB);
    }

    n += ocdwarf_printf_merit(p, die, attr, nattr, e);
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_printf_merit(handle_t p, Dwarf_Die die, Dwarf_Attribute attr, Dwarf_Half nattr, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    const imode_t TRY_HEX      = PICK_ENHANCED(oc, USE_FHEX32, USE_FHEX);
    const imode_t TRY_COLON    = PICK_ENHANCED(oc, USE_NONE,   USE_COLON);
    const imode_t TRY_HEXDEC   = PICK_ENHANCED(oc, USE_DEC,    USE_FHEX);
    const imode_t TRY_DECHEX16 = PICK_ENHANCED(oc, USE_FHEX16, USE_DEC);
    const imode_t TRY_DECHEX32 = PICK_ENHANCED(oc, USE_FHEX32, USE_DEC);

    Dwarf_Half nform = 0;
    x = dwarf_whatform(attr, &nform, e);
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x) && e) {
        ocdwarf_dealloc_error(p, e);
      }

      ocdwarf_object_finish(p);
      printf_x("dwarf_whatform");
    }

    Dwarf_Off offset = 0;
    x = dwarf_attr_offset(die, attr, &offset, e);
    if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
      printf_e("dwarf_attr_offset failed! errcode %d", x);
#endif
      return OCDWARF_ERRCODE(x, n);
    }

    if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
        n += ocdwarf_printf_HEX(p, offset, USE_NONE);
      }
    } else {
      n += ocdwarf_printf_HEX(p, offset, USE_NONE);
    }

    n += ocdwarf_printf_AT(p, nattr, TRY_COLON);

    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      n += ocdwarf_printf_FORM(p, nform, USE_COLON);
    }

    if (isused(ecFORMSTRING, nform)) {
      char *str = NULL;
      x = dwarf_formstring(attr, &str, e);
      if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("dwarf_formstring failed! errcode %d", x);
#endif
        return OCDWARF_ERRCODE(x, n);
      }

//      Dwarf_Off soffset = 0;
//      Dwarf_Bool isinfo = FALSE;
//      x = dwarf_global_formref_b(attr, &soffset, &isinfo, e);
//      if (IS_DLV_ANY_ERROR(x)) {
//        printf_e("dwarf_formstring failed! errcode %d", x);
//        return OCDWARF_ERRCODE(x, n);
//      }

//      n += printf_text("indirect string, offset", USE_LT | USE_RBLT | USE_COLON | USE_SPACE);
//      n += printf_nice(soffset, USE_FHEX | USE_RBRT | USE_COLON);

      n += printf_text(str, USE_LT | USE_SPACE);
      ocdwarf_dealloc(p, str, DW_DLA_STRING);
    } else if (isused(ecFORMUDATA, nform)) {
      Dwarf_Unsigned value = 0;
      x = dwarf_formudata(attr, &value, e);
      if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("dwarf_formudata failed! errcode %d", x);
#endif
        return OCDWARF_ERRCODE(x, n);
      }

      if (DW_AT_decl_line == nattr) {
        nline = value;
      } else if (DW_AT_decl_column == nattr) {
        ncolumn = value;
      }

      if (DW_AT_high_pc == nattr) {
        if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED | OPTDWARF_VERBOSE)) {
          n += printf_text("offset-from-lowpc", USE_LT | USE_SPACE | USE_TB);
          n += printf_nice(value, TRY_HEXDEC);
          n += printf_text("highpc", USE_LT | USE_SPACE | USE_TBLT | USE_COLON);
          n += ocdwarf_printf_ADDR(p, low_pc_addr + value, USE_TBRT);
        } else {
          n += printf_nice(value, TRY_HEXDEC);
        }
      } else if (DW_AT_language == nattr) {
        n += ocdwarf_printf_LANG(p, value, USE_NONE);
      } else if (isused(ecATE, nattr)) {
        n += ocdwarf_printf_ATE(p, value, USE_NONE);
      } else if (isused(ecATDEC, nattr)) {
        n += printf_nice(value, USE_DEC);
        if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
          if (isused(ecATDEC8, nattr) && (CHAR_MAX <= value)) {
            n += printf_nice(value, USE_SDEC8 | USE_RB);
          } else if (isused(ecATDEC16, nattr) && (SHRT_MAX <= value)) {
            n += printf_nice(value, USE_SDEC16 | USE_RB);
          }
        }
      } else if (isused(ecATHEX32, nattr)) {
        n += printf_nice(value, TRY_DECHEX32);
      } else if (isused(ecATSRCFILE, nattr)) {
        if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
          n += ocdwarf_printf_SRCFILE(p, value, TRY_DECHEX32);
        } else {
          n += printf_nice(value, USE_DEC);
        }
      } else {
        n += printf_nice(value, USE_FHEX16);
      }
    } else if (isused(ecFORMADDR, nform)) {
      Dwarf_Addr addr = 0;
      x = dwarf_formaddr(attr, &addr, e);
      if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("dwarf_formaddr failed! errcode %d", x);
#endif
        return OCDWARF_ERRCODE(x, n);
      }

      if (DW_AT_low_pc == nattr) {
        low_pc_addr = addr;
      }

      n += ocdwarf_printf_ADDR(p, addr, USE_NONE);
    } else if (isused(ecFORMBOOL, nform)) {
      Dwarf_Unsigned value = 0;
      x = dwarf_formudata(attr, &value, e);
      if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("dwarf_formudata failed! errcode %d", x);
#endif
        return OCDWARF_ERRCODE(x, n);
      }

      n += printf_nice(value, USE_YESNO);
    } else if (isused(ecFORMGREF, nform)) {
      Dwarf_Unsigned value = 0;
      x = dwarf_global_formref(attr, &value, e);
      if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("dwarf_global_formref failed! errcode %d", x);
#endif
        return OCDWARF_ERRCODE(x, n);
      }

      n += printf_nice(value, TRY_HEX);
    } else if (isused(ecFORMOFFSET, nform)) {
      Dwarf_Off offset = 0;
      Dwarf_Bool isinfo = FALSE;
      if (DW_AT_type == nattr) {
        x = dwarf_dietype_offset(die, &offset, &isinfo, e);
        if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
          printf_e("dwarf_dietype_offset failed! errcode %d", x);
#endif
          return OCDWARF_ERRCODE(x, n);
        }
      } else {
        x = dwarf_global_formref_b(attr, &offset, &isinfo, e);
        if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
          printf_e("dwarf_global_formref_b failed! errcode %d", x);
#endif
          return OCDWARF_ERRCODE(x, n);
        }
      }

      n += printf_nice(offset, TRY_HEX | USE_TB);
      if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
        n += printf_nice(isinfo, USE_BOOL);
      }

      if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
        Dwarf_Die tdie = 0;
        x = dwarf_offdie_b(ocget(p, OPCODE_DWARF_DEBUG), offset, isinfo, &tdie, e);
        if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
          printf_e("dwarf_offdie_b failed! errcode %d", x);
#endif
          return OCDWARF_ERRCODE(x, n);
        }

        char *name = 0;
        int x0 = dwarf_diename(tdie, &name, e);
        if (IS_DLV_ERROR(x0)) {
#ifdef OPCODE_DWARF_DEBUGX
          printf_e("dwarf_diename failed! errcode %d", x0);
#endif
          return OCDWARF_ERRCODE(x0, n);
        } else if (IS_DLV_OK(x0)) {
          n += printf_text(name, USE_LT | USE_SPACE | USE_TB);
          ocdwarf_dealloc(p, name, DW_DLA_STRING);
        }
      }
    } else if (isused(ecFORMBLOCK, nform)) {
      Dwarf_Block *block = 0;
      int x0 = dwarf_formblock(attr, &block, e);
      if (IS_DLV_ERROR(x0)) {
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("dwarf_formblock failed! errcode %d", x0);
#endif
        return OCDWARF_ERRCODE(x0, n);
      } else if (IS_DLV_OK(x0) && block && 0 != block->bl_len) {
        n += printf_nice(block->bl_len, TRY_DECHEX16);
        n += printf_text("byte block", USE_LT | USE_SPACE | USE_COLON);
        n += printf_hurt(block->bl_data, block->bl_len, USE_HEX | USE_SPACE | USE_COLON | USE_0x);
        if (block->bl_len >= 1) {
          uchar_t v0 = CAST(puchar_t, block->bl_data)[0];
          n += ocdwarf_printf_OP(p, v0, block->bl_len == 1 ? USE_SPACE : USE_SPACE | TRY_COLON);
          if (block->bl_len >= 2) {
            uchar_t v1 = CAST(puchar_t, block->bl_data)[1];
            if (DW_OP_fbreg == v0) {
              n += printf_nice(sleb128_decode(CAST(puchar_t, block->bl_data) + 1, block->bl_len - 1), USE_DEC);
            } else if (DW_OP_breg5 == v0) {
              n += printf_nice(v1, USE_SDEC8 | USE_NOSPACE);
            } else if (block->bl_len >= 5) {
              n += printf_hurt(CAST(puchar_t, block->bl_data) + 3, 2, USE_HEX | USE_SPACE | USE_0x);
              n += printf_hurt(CAST(puchar_t, block->bl_data) + 2, 1, USE_HEX);
              n += printf_hurt(CAST(puchar_t, block->bl_data) + 1, 1, USE_HEX);
            }
          }
        }

        ocdwarf_dealloc(p, block, DW_DLA_BLOCK);
      }
    }

    n += printf_eol();
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_printf_value(handle_t p, Dwarf_Die die, Dwarf_Half nattr, pdwarf_srcfiles_t sf, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    Dwarf_Attribute attr = 0;
    x = dwarf_attr(die, nattr, &attr, e);
    if (IS_DLV_OK(x)) {
      n += printf_pack(MAXMERIT);
      n += ocdwarf_printf_merit(p, die, attr, nattr, e);
      dwarf_dealloc_attribute(attr);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

static int ocdwarf_printf_name(handle_t p, Dwarf_Die die, Dwarf_Attribute attr, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    Dwarf_Half attrnum = 0;
    x = dwarf_whatattr(attr, &attrnum, e);
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x) && e) {
        ocdwarf_dealloc_error(p, e);
      }
      ocdwarf_object_finish(p);
      printf_x("dwarf_whatattr");
    }

    Dwarf_Half formnum = 0;
    x = dwarf_whatform(attr, &formnum, e);
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x) && e) {
        ocdwarf_dealloc_error(p, e);
      }
      ocdwarf_object_finish(p);
      printf_x("dwarf_whatform");
    }

    pocdwarf_t ws = ocget(p, OPCODE_DWARF);
    enum Dwarf_Form_Class cl = dwarf_get_form_class(ws->cu_version_stamp, attrnum, ws->cu_offset_size, formnum);
    if (DW_FORM_CLASS_STRING == cl) {
      char *stringval = 0;
      x = dwarf_die_text(die, attrnum, &stringval, e);
      if (IS_DLV_OK(x)) {
        n += printf_text("CLASS", USE_LT | USE_COLON);
        n += printf_text(stringval, USE_LT | USE_SPACE | USE_EOL);
        ocdwarf_dealloc(p, stringval, DW_DLA_STRING);
      }
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_printf_groups(handle_t p, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    Dwarf_Unsigned group_count = 0;
    Dwarf_Unsigned section_count = 0;
    Dwarf_Unsigned selected_group = 0;
    Dwarf_Unsigned group_map_entry_count = 0;

    x = dwarf_sec_group_sizes(ocget(p, OPCODE_DWARF_DEBUG), &section_count, &group_count, &selected_group,
                     &group_map_entry_count, e);

    if (IS_DLV_OK(x)) {
      if (group_count == 1 && DW_GROUPNUMBER_BASE == selected_group) {

      } else if (group_count > 0) {
        n += printf_text("Section Groups data", USE_LT | USE_EOL);
        n += printf_text("Number of Elf-like sections", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXGROUP));
        n += printf_nice(section_count, USE_DEC | USE_EOL);
        n += printf_text("Number of groups", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXGROUP));
        n += printf_nice(group_count, USE_DEC | USE_EOL);
        n += printf_text("Group to print", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXGROUP));
        n += ocdwarf_printf_GNUM(p, selected_group, USE_SPECIAL | USE_EOL);
        n += printf_text("Count of map entries", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXGROUP));
        n += printf_nice(group_map_entry_count, USE_DEC | USE_EOL);

        MALLOCA(Dwarf_Unsigned, sec_nums, group_map_entry_count);
        MALLOCA(Dwarf_Unsigned, group_nums, group_map_entry_count);
        MALLOCA(const char *, sec_names, group_map_entry_count);

        x = dwarf_sec_group_map(ocget(p, OPCODE_DWARF_DEBUG), group_map_entry_count,
                     group_nums, sec_nums, sec_names, e);

        if (IS_DLV_OK(x)) {
          for (Dwarf_Unsigned i = 0; i < group_map_entry_count; ++i) {
            if (i == 0) {
              n += printf_text("[index]  group section", USE_LT | USE_TAB | USE_EOL);
            }

            n += printf_nice(i, USE_DEC5 | USE_SB);
            n += printf_nice(group_nums[i], USE_DEC4);
            n += printf_nice(sec_nums[i], USE_DEC4);
            n += printf_text(sec_names[i], USE_LT | USE_SPACE);
            n += printf_eol();
          }
        }
      }
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_printf_names(handle_t p, Dwarf_Die die, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    Dwarf_Signed attrcount = 0;
    Dwarf_Attribute *attrbuf = 0;
    x = dwarf_attrlist(die, &attrbuf, &attrcount, e);
    if (IS_DLV_ERROR(x)) {
      ocdwarf_dealloc_error(p, e);
    } else if (IS_DLV_OK(x)) {
      for (Dwarf_Signed i = 0; i < attrcount; ++i) {
        n += ocdwarf_printf_name(p, die, attrbuf[i], e);
      }
      ocdwarf_dealloc(p, attrbuf, DW_DLA_LIST);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_printf_cu(handle_t p, Dwarf_Die die, Dwarf_Half tag,
              Dwarf_Bool isinfo, int level, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p)) {
    Dwarf_Off overall_offset = 0;
    x = dwarf_dieoffset(die, &overall_offset, e);
    if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
      printf_e("dwarf_dieoffset failed! errcode %d", x);
#endif
      return OCDWARF_ERRCODE(x, n0);
    }

    Dwarf_Off offset = 0;
    x = dwarf_die_CU_offset(die, &offset, e);
    if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
      printf_e("dwarf_die_CU_offset failed! res %d", x);
#endif
      return OCDWARF_ERRCODE(x, n0);
    }

    n0 += printf_text("COMPILE_UNIT<header overall offset =", USE_LT);
    n0 += printf_nice(overall_offset - offset, USE_FHEX32 | USE_TBRT | USE_COLON | USE_EOL);

    int n1 = ocdwarf_printf_sp(p, die, tag, isinfo, level, e);
    if (ECODE_ISEVIL(n1)) return n1;

    n0 += ocdwarf_sfcreate(p, die, e);
  }

  return OCDWARF_ERRCODE(x, n0);
}

int ocdwarf_printf_sp(handle_t p, Dwarf_Die die, Dwarf_Half tag,
              Dwarf_Bool isinfo, int level, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    char *name = 0;
    x = dwarf_diename(die, &name, e);
    if (IS_DLV_ERROR(x)) {
      ocdwarf_object_finish(p);
      printf_x("dwarf_diename, level %d", level);
    } else if (IS_DLV_NO_ENTRY(x)) {
      name = "<no DW_AT_name attr>";
    }

    Dwarf_Signed cattr = 0;
    Dwarf_Attribute *pattr = 0;
    x = dwarf_attrlist(die, &pattr, &cattr, e);
    if (IS_DLV_NO_ENTRY(x)) {
      return n0;
    } else if (IS_DLV_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
      printf_e("dwarf_attrlist failed! errcode %d", x);
#endif
      return OCDWARF_ERRCODE(x, n0);
    }

    Dwarf_Off overall_offset = 0;
    x = dwarf_dieoffset(die, &overall_offset, e);
    if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
      printf_e("dwarf_dieoffset failed! errcode %d", x);
#endif
      return OCDWARF_ERRCODE(x, n0);
    }

    n0 += ocdwarf_printf_DEC(p, level, USE_NONE);
    n0 += ocdwarf_printf_HEX(p, overall_offset, USE_NOSPACE);
    n0 += ocdwarf_printf_TAG(p, tag, USE_NONE);
    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      n0 += printf_text(name, USE_LT | USE_SPACE | USE_SQ);
    }
    n0 += printf_eol();

    for (Dwarf_Signed i = 0; i < cattr; ++i) {
      int n1 = ocdwarf_printf_worth(p, die, pattr[i], i, e);
      if (ECODE_ISEVIL(n1)) {
        ocdwarf_dealloc_attribute(p, pattr, cattr);
        return n1;
      }
      n0 += n1;
    }

    ocdwarf_dealloc_attribute(p, pattr, cattr);
    n0 += printf_eol();
  }

  return OCDWARF_ERRCODE(x, n0);
}

int ocdwarf_printf(handle_t p, Dwarf_Die die, Dwarf_Bool isinfo, int level, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    char *name = 0;
    x = dwarf_diename(die, &name, e);
    if (IS_DLV_ERROR(x)) {
      ocdwarf_object_finish(p);
      printf_x("dwarf_diename, level %d", level);
    } else if (IS_DLV_NO_ENTRY(x)) {
      name = "<no DW_AT_name attr>";
    }

    Dwarf_Half tag = 0;
    x = dwarf_tag(die, &tag, e);
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x) && e) {
        ocdwarf_dealloc_error(p, e);
      }
      ocdwarf_object_finish(p);
      printf_x("dwarf_tag, level %d", level);
    }

    if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
      n += ocdwarf_printf_names(p, die, e);
    }

    if (tag == DW_TAG_subprogram) {
      n += ocdwarf_printf_me(p, level, "subprogram", name, USE_EOL);
      n += ocdwarf_printf_sp(p, die, tag, isinfo, level, e);
    } else if (tag == DW_TAG_compile_unit || tag == DW_TAG_partial_unit || tag == DW_TAG_type_unit) {
      ocdwarf_sfreset(p);
      n += ocdwarf_printf_me(p, level, "source file", name, USE_EOL);
      n += ocdwarf_printf_cu(p, die, tag, isinfo, level, e);
    } else {
      n += ocdwarf_printf_sp(p, die, tag, isinfo, level, e);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

