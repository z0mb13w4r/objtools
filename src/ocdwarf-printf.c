#include "decode.h"
#include "options.h"
#include "ocdwarf-printf.h"

Dwarf_Unsigned nline = 0;
Dwarf_Unsigned ncolumn = 0;
Dwarf_Addr low_pc_addr = 0;

int ocdwarf_printf_me(handle_t p, const int x, const char *y, const char *z, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      n += printf_nice(x, USE_DEC2 | USE_TB | USE_NOSPACE);
      n += printf_text(y, USE_LT | USE_SPACE | USE_COLON | SET_PAD(23));
      n += printf_text(z, USE_LT | USE_SPACE | USE_DQ | mode);
    }
  }

  return n;
}

int ocdwarf_printf_pluck(handle_t p, const pconvert_t z, const pick_t x, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      n += printf_nice(x, USE_FHEX16);
    } else if (MODE_ISSET(USE_SPECIAL, mode)) {
      n += printf_nice(x, USE_FHEX8);
    }

    n += printf_pick(z, x, USE_SPACE | (mode & ~USE_SPECIAL));
  }

  return n;
}

int ocdwarf_printf_idx(handle_t p, const uint64_t v, const imode_t mode) {
  return printf_nice(v, USE_DEC2 | USE_TB | mode);
}

int ocdwarf_printf_num(handle_t p, const uint64_t v, const imode_t mode) {
  return printf_nice(v, USE_FHEX32 | USE_TB | mode);
}

int ocdwarf_printf_AT(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pluck(p, zDWAT, v, mode | SET_PAD(30));
}

int ocdwarf_printf_OP(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pluck(p, zDWOP, v, mode);
}

int ocdwarf_printf_ATE(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pluck(p, zDWATE, v, mode);
}

int ocdwarf_printf_TAG(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pluck(p, zDWTAG, v, mode);
}

int ocdwarf_printf_FORM(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pluck(p, zDWFORM, v, mode | SET_PAD(22));
}

int ocdwarf_printf_LANG(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pluck(p, zDWLANG, v, mode);
}

int ocdwarf_printf_MACRO(handle_t p, const uint64_t v, const imode_t mode) {
  if (0 == v && MODE_ISSET(USE_SPECIAL, mode)) {
    return printf_nice(v, USE_FHEX8) + printf_text("end-of-macros", USE_LT | USE_SPACE);
  }

  return ocdwarf_printf_pluck(p, zDWMACRO, v, mode);
}

int ocdwarf_printf_srcfile(handle_t p, const uint32_t v, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    pdwarf_srcfiles_t sf = ocget(p, OPCODE_DWARF_SRCFILES);
    if (sf && IS_DLV_OK(sf->status) && (0 != sf->size) && ((v - 1) < sf->size)) {
      n += printf_nice(v, mode);
      n += printf_text(sf->data[v - 1], USE_LT | USE_SPACE);
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
      printf_e("dwarf_whatattr failed! errcode %d", x);
      return OCDWARF_ERRCODE(x, n);
    }

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      n += printf_nice(index, USE_DEC2 | USE_SB | USE_TAB);
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

    Dwarf_Half nform = 0;
    x = dwarf_whatform(attr, &nform, e);
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x) && e) {
        ocdwarf_dealloc_error(p, e);
      }

      ocdwarf_object_finish(p);
      printf_x("dwarf_whatform");
    }

    n += printf_pack(4);
    n += ocdwarf_printf_AT(p, nattr, USE_NONE);

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      n += ocdwarf_printf_FORM(p, nform, USE_NONE);
    }

    if (isused(zFORMSTRING, nform)) {
      char *str = NULL;
      x = dwarf_formstring(attr, &str, e);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_e("dwarf_formstring failed! errcode %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      n += printf_text(str, USE_LT | USE_SPACE);
    } else if (isused(zFORMUDATA, nform)) {
      Dwarf_Unsigned value = 0;
      x = dwarf_formudata(attr, &value, e);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_e("dwarf_formudata failed! errcode %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      if (DW_AT_decl_line == nattr) {
        nline = value;
      } else if (DW_AT_decl_column == nattr) {
        ncolumn = value;
      }

      if (DW_AT_high_pc == nattr) {
        n += printf_text("offset-from-lowpc", USE_LT | USE_SPACE | USE_TB);
        n += printf_nice(value, USE_DEC);
        n += printf_text("highpc", USE_LT | USE_SPACE | USE_TBLT | USE_COLON);
        n += printf_nice(low_pc_addr + value, USE_FHEX32 | USE_TBRT);
      } else if (DW_AT_language == nattr) {
        n += ocdwarf_printf_LANG(p, value, USE_NONE);
      } else if (isused(zATE, nattr)) {
        n += ocdwarf_printf_ATE(p, value, USE_NONE);
      } else if (isused(zATDEC, nattr)) {
        n += printf_nice(value, USE_DEC);
        if (isused(zATDEC8, nattr) && (CHAR_MAX <= value)) {
          n += printf_nice(value, USE_SDEC8 | USE_RB);
        } else if (isused(zATDEC16, nattr) && (SHRT_MAX <= value)) {
          n += printf_nice(value, USE_SDEC16 | USE_RB);
        }
      } else if (isused(zATHEX32, nattr)) {
        n += printf_nice(value, USE_FHEX32);
      } else if (isused(zATSRCFILE, nattr)) {
        n += ocdwarf_printf_srcfile(p, value, USE_FHEX32);
      } else {
        n += printf_nice(value, USE_FHEX16);
      }
    } else if (isused(zFORMADDR, nform)) {
      Dwarf_Addr addr = 0;
      x = dwarf_formaddr(attr, &addr, e);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_e("dwarf_formaddr failed! errcode %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      if (DW_AT_low_pc == nattr) {
        low_pc_addr = addr;
      }

      n += printf_nice(addr, USE_FHEX32);
    } else if (isused(zFORMBOOL, nform)) {
      Dwarf_Unsigned value = 0;
      x = dwarf_formudata(attr, &value, e);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_e("dwarf_formudata failed! errcode %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      n += printf_nice(value, USE_YESNO);
    } else if (isused(zFORMGREF, nform)) {
      Dwarf_Unsigned value = 0;
      x = dwarf_global_formref(attr, &value, e);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_e("dwarf_global_formref failed! errcode %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      n += printf_nice(value, USE_FHEX32);
    } else if (isused(zFORMOFFSET, nform)) {
      Dwarf_Off offset = 0;
      Dwarf_Bool isinfo = FALSE;
      if (DW_AT_type == nattr) {
        x = dwarf_dietype_offset(die, &offset, &isinfo, e);
        if (IS_DLV_ANY_ERROR(x)) {
          printf_e("dwarf_dietype_offset failed! errcode %d", x);
          return OCDWARF_ERRCODE(x, n);
        }
      } else {
        x = dwarf_global_formref_b(attr, &offset, &isinfo, e);
        if (IS_DLV_ANY_ERROR(x)) {
          printf_e("dwarf_global_formref_b failed! errcode %d", x);
          return OCDWARF_ERRCODE(x, n);
        }
      }

      n += printf_nice(offset, USE_FHEX32 | USE_TB);
      if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
        n += printf_nice(isinfo, USE_BOOL);
      }

      Dwarf_Die tdie = 0;
      x = dwarf_offdie_b(ocget(p, OPCODE_DWARF_DEBUG), offset, isinfo, &tdie, e);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_e("dwarf_offdie_b failed! errcode %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      char *name = 0;
      int x0 = dwarf_diename(tdie, &name, e);
      if (IS_DLV_ERROR(x0)) {
        printf_e("dwarf_diename failed! errcode %d", x0);
        return OCDWARF_ERRCODE(x0, n);
      } else if (IS_DLV_OK(x0)) {
        n += printf_text("Refers to", USE_LT | USE_SPACE | USE_COLON);
        n += printf_text(name, USE_LT | USE_SPACE);
      }
    } else if (isused(zFORMBLOCK, nform)) {
      Dwarf_Block *block = 0;
      int x0 = dwarf_formblock(attr, &block, e);
      if (IS_DLV_ERROR(x0)) {
        printf_e("dwarf_formblock failed! errcode %d", x0);
        return OCDWARF_ERRCODE(x0, n);
      } else if (IS_DLV_OK(x0) && block && 0 != block->bl_len) {
        n += printf_text("len", USE_LT | USE_SPACE);
        n += printf_nice(block->bl_len, USE_FHEX16 | USE_COLON);
        n += printf_hurt(block->bl_data, block->bl_len, USE_HEX | USE_SPACE | USE_COLON | USE_0x);
        if (block->bl_len >= 1) {
          uchar_t v0 = CAST(puchar_t, block->bl_data)[0];
          n += ocdwarf_printf_OP(p, v0, USE_SPACE);
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
      n += ocdwarf_printf_merit(p, die, attr, nattr, e);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

static int ocdwarf_printf_name(handle_t p, handle_t s, Dwarf_Die die, Dwarf_Attribute attr, Dwarf_Error *e) {
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

int ocdwarf_printf_names(handle_t p, handle_t s, Dwarf_Die die, Dwarf_Error *e) {
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
        n += ocdwarf_printf_name(p, s, die, attrbuf[i], e);
      }
      ocdwarf_dealloc(p, attrbuf, DW_DLA_LIST);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_printf_cu(handle_t p, handle_t s, Dwarf_Die die, Dwarf_Half tag,
              Dwarf_Bool isinfo, int level, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    Dwarf_Off overall_offset = 0;
    x = dwarf_dieoffset(die, &overall_offset, e);
    if (IS_DLV_ANY_ERROR(x)) {
      printf_e("dwarf_dieoffset failed! errcode %d", x);
      return OCDWARF_ERRCODE(x, n0);
    }

    Dwarf_Off offset = 0;
    x = dwarf_die_CU_offset(die, &offset, e);
    if (IS_DLV_ANY_ERROR(x)) {
      printf_e("dwarf_die_CU_offset failed! res %d", x);
      return OCDWARF_ERRCODE(x, n0);
    }

    n0 += printf_text("COMPILE_UNIT<header overall offset =", USE_LT);
    n0 += printf_nice(overall_offset - offset, USE_FHEX32 | USE_TBRT | USE_COLON | USE_EOL);

    int n1 = ocdwarf_printf_sp(p, s, die, tag, isinfo, level, e);
    if (OCDWARF_ISERRCODE(n1)) return n1;

    n0 += ocdwarf_sfcreate(p, die, e);
  }

  return OCDWARF_ERRCODE(x, n0);
}

int ocdwarf_printf_sp(handle_t p, handle_t s, Dwarf_Die die, Dwarf_Half tag,
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
      printf_e("dwarf_attrlist failed! errcode %d", x);
      return OCDWARF_ERRCODE(x, n0);
    }

    Dwarf_Off overall_offset = 0;
    x = dwarf_dieoffset(die, &overall_offset, e);
    if (IS_DLV_ANY_ERROR(x)) {
      printf_e("dwarf_dieoffset failed! errcode %d", x);
      return OCDWARF_ERRCODE(x, n0);
    }

    n0 += ocdwarf_printf_idx(p, level, USE_NONE);
    n0 += ocdwarf_printf_num(p, overall_offset, USE_NOSPACE);
    n0 += ocdwarf_printf_TAG(p, tag, USE_NONE);
    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      n0 += printf_text(name, USE_LT | USE_SPACE | USE_SQ);
    }
    n0 += printf_eol();

    for (Dwarf_Signed i = 0; i < cattr; ++i) {
      int n1 = ocdwarf_printf_worth(p, die, pattr[i], i, e);
      if (OCDWARF_ISERRCODE(n1)) {
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

int ocdwarf_printf(handle_t p, handle_t s,
              Dwarf_Die die, Dwarf_Bool isinfo, int level, Dwarf_Error *e) {
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

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      n += ocdwarf_printf_names(p, s, die, e);
    }

    if (tag == DW_TAG_subprogram) {
      n += ocdwarf_printf_me(p, level, "subprogram", name, USE_EOL);
      n += ocdwarf_printf_sp(p, s, die, tag, isinfo, level, e);
    } else if (tag == DW_TAG_compile_unit || tag == DW_TAG_partial_unit || tag == DW_TAG_type_unit) {
      ocdwarf_sfreset(p);
      n += ocdwarf_printf_me(p, level, "source file", name, USE_EOL);
      n += ocdwarf_printf_cu(p, s, die, tag, isinfo, level, e);
    } else {
      n += ocdwarf_printf_sp(p, s, die, tag, isinfo, level, e);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

