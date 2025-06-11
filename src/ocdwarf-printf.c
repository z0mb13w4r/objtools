#include "options.h"
#include "ocdwarf-printf.h"

int ocdwarf_printf_me(handle_t p, const int x, const char *y, const char *z, const imode_t mode) {
  int n = 0;
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
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
    popcode_t oc = CAST(popcode_t, p);
    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      n += printf_nice(x, USE_FHEX16);
    }
    n += printf_pick(z, x, USE_SPACE | mode);
  }

  return n;
}

int ocdwarf_printf_idx(handle_t p, const uint64_t v, const imode_t mode) {
  return printf_nice(v, USE_DEC2 | USE_TB | mode);
}

int ocdwarf_printf_AT(handle_t p, const uint64_t v, const imode_t mode) {
  return ocdwarf_printf_pluck(p, zDWAT, v, mode | SET_PAD(27));
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

int ocdwarf_printf_addr(handle_t p, const uint64_t v, const imode_t mode) {
  return printf_nice(v, USE_FHEX32 | USE_TB | mode);
}

int ocdwarf_printf_srcfiles(handle_t p, pdwarf_srcfiles_t sf) {
  int n = 0;
  if (isopcode(p) && sf) {
    if (IS_DLV_OK(sf->status) && 0 != sf->size) {
      for (Dwarf_Signed i = 0; i < sf->size; ++i) {
        n += printf_nice(i, USE_DEC3 | USE_TB | USE_COLON);
        n += printf_text(sf->data[i], USE_LT | USE_EOL);
      }
    }
  }

  return n;
}

int ocdwarf_printf_srcfile(handle_t p, const uint32_t x, pdwarf_srcfiles_t sf, const imode_t mode) {
  int n = 0;
  if (isopcode(p) && sf) {
    if (IS_DLV_OK(sf->status) && (0 != sf->size) && ((x - 1) < sf->size)) {
      n += printf_nice(x, USE_FHEX32);
      n += printf_text(sf->data[x - 1], USE_LT | USE_SPACE | mode);
    }
  }

  return n;
}

Dwarf_Addr low_pc_addr = 0;

int ocdwarf_printf_worth(handle_t p, Dwarf_Die die, Dwarf_Attribute attr, Dwarf_Signed index, pdwarf_srcfiles_t sf, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);

    Dwarf_Half nattr = 0;
    x = dwarf_whatattr(attr, &nattr, e);
    if (IS_DLV_ANY_ERROR(x)) {
      printf_e("dwarf_whatattr failed! errcode %d", x);
      return OCDWARF_ERRCODE(x, n);
    }

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      n += printf_nice(index, USE_DEC2 | USE_SB | USE_TAB);
    }

    n += ocdwarf_printf_merit(p, die, attr, nattr, sf, e);
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_printf_merit(handle_t p, Dwarf_Die die, Dwarf_Attribute attr, Dwarf_Half nattr, pdwarf_srcfiles_t sf, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);

    Dwarf_Half nform = 0;
    x = dwarf_whatform(attr, &nform, e);
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x) && e) {
        dwarf_dealloc_error(oc->items[OPCODE_DWARF], *e);
      }

      dwarf_object_finish(oc->items[OPCODE_DWARF]);
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
      } else if (isused(zATHEX32, nattr)) {
        n += printf_nice(value, USE_FHEX32);
      } else if (isused(zATSRCFILE, nattr)) {
        n += ocdwarf_printf_srcfile(p, value, sf, USE_NONE);
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
    } else if (isused(zFORMGREF, nform)) {
      Dwarf_Unsigned value = 0;
      x = dwarf_global_formref(attr, &value, e);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_e("dwarf_global_formref failed! errcode %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      n += printf_nice(value, USE_FHEX32);
    } else if (DW_AT_type == nattr) {
      Dwarf_Off offset = 0;
      Dwarf_Bool isinfo = FALSE;
      x = dwarf_dietype_offset(die, &offset, &isinfo, e);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_e("dwarf_dietype_offset failed! errcode %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      n += printf_nice(offset, USE_FHEX32);
      n += printf_nice(isinfo, USE_BOOL);
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
      n += ocdwarf_printf_merit(p, die, attr, nattr, sf, e);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

static int ocdwarf_printf_name(handle_t p, handle_t s, Dwarf_Die die, Dwarf_Attribute attr, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);

    Dwarf_Half attrnum = 0;
    x = dwarf_whatattr(attr, &attrnum, e);
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x) && e) {
        dwarf_dealloc_error(oc->items[OPCODE_DWARF], *e);
      }
      dwarf_object_finish(oc->items[OPCODE_DWARF]);
      printf_x("dwarf_whatattr");
    }

    Dwarf_Half formnum = 0;
    x = dwarf_whatform(attr, &formnum, e);
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x) && e) {
        dwarf_dealloc_error(oc->items[OPCODE_DWARF], *e);
      }
      dwarf_object_finish(oc->items[OPCODE_DWARF]);
      printf_x("dwarf_whatform");
    }

    enum Dwarf_Form_Class cl = dwarf_get_form_class(cu_version_stamp, attrnum, cu_offset_size, formnum);
    if (DW_FORM_CLASS_STRING == cl) {
      char *stringval = 0;
      x = dwarf_die_text(die, attrnum, &stringval, e);
      if (IS_DLV_OK(x)) {
        n += printf_text("CLASS", USE_LT | USE_COLON);
        n += printf_text(stringval, USE_LT | USE_SPACE | USE_EOL);
        dwarf_dealloc(oc->items[OPCODE_DWARF], stringval, DW_DLA_STRING);
      }
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_printf_names(handle_t p, handle_t s, Dwarf_Die die, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);

    Dwarf_Signed attrcount = 0;
    Dwarf_Attribute *attrbuf = 0;
    x = dwarf_attrlist(die, &attrbuf, &attrcount, e);
    if (IS_DLV_ERROR(x)) {
      dwarf_dealloc_error(oc->items[OPCODE_DWARF], *e);
    } else if (IS_DLV_OK(x)) {
      for (Dwarf_Signed i = 0; i < attrcount; ++i) {
        n += ocdwarf_printf_name(p, s, die, attrbuf[i], e);
      }
      dwarf_dealloc(oc->items[OPCODE_DWARF], attrbuf, DW_DLA_LIST);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_printf_one(handle_t p, handle_t s, Dwarf_Die die, int level, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      char *name = 0;
      x = dwarf_diename(die, &name, e);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_e("dwarf_diename, errcode %d", x);
        return OCDWARF_ERRCODE(x, n0);
      }

      n0 += printf_text(name, USE_LT | USE_EOL);
    }

    Dwarf_Half tag = 0;
    x = dwarf_tag(die, &tag, e);
    if (IS_DLV_ANY_ERROR(x)) {
      printf_e("dwarf_tag failed! errcode %d", x);
      return OCDWARF_ERRCODE(x, n0);
    }

    Dwarf_Off overall_offset = 0;
    x = dwarf_dieoffset(die, &overall_offset, e);
    if (IS_DLV_ANY_ERROR(x)) {
      printf_e("dwarf_dieoffset failed! errcode %d", x);
      return OCDWARF_ERRCODE(x, n0);
    }

    Dwarf_Off offset = 0;
    x = dwarf_die_CU_offset(die, &offset, e);
    if (IS_DLV_ANY_ERROR(x)) {
      printf_e("dwarf_die_CU_offset failed! errcode %d", x);
      return OCDWARF_ERRCODE(x, n0);
    }

    n0 += printf_text("COMPILE_UNIT<header overall offset =", USE_LT);
    n0 += printf_nice(overall_offset - offset, USE_FHEX32 | USE_TBRT | USE_COLON | USE_EOL);
    n0 += ocdwarf_printf_idx(p, level, USE_NONE);
    n0 += ocdwarf_printf_addr(p, 0xffffffff, USE_NONE);
    n0 += ocdwarf_printf_TAG(p, tag, USE_EOL);

    Dwarf_Signed attrcount = 0;
    Dwarf_Attribute *attrbuf = 0;
    x = dwarf_attrlist(die, &attrbuf, &attrcount, e);
    if (IS_DLV_ANY_ERROR(x)) {
      printf_e("dwarf_attrlist failed! errcode %d", x);
      return OCDWARF_ERRCODE(x, n0);
    }

    for (Dwarf_Signed i = 0; i < attrcount; ++i) {
      int n1 = ocdwarf_printf_worth(p, die, attrbuf[i], i, NULL, e);
      if (OCDWARF_ISERRCODE(n1)) {
        ocdwarf_dealloc(p, s, attrbuf, attrcount, 0);
        return n1;
      }
      n0 += n1;
    }

    ocdwarf_dealloc(p, s, attrbuf, attrcount, 0);
  }

  return OCDWARF_ERRCODE(x, n0);
}

int ocdwarf_printf_cu(handle_t p, handle_t s, Dwarf_Die die, Dwarf_Half tag,
                  Dwarf_Bool isinfo, int level, pdwarf_srcfiles_t sf, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p) && sf) {
    popcode_t oc = CAST(popcode_t, p);

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
    n0 += ocdwarf_printf_idx(p, level, USE_NONE);
    n0 += ocdwarf_printf_addr(p, 0xffffffff, USE_NONE);
    n0 += ocdwarf_printf_TAG(p, tag, USE_EOL);

    Dwarf_Signed attrcount = 0;
    Dwarf_Attribute *attrbuf = 0;
    x = dwarf_attrlist(die, &attrbuf ,&attrcount, e);
    if (IS_DLV_ANY_ERROR(x)) return OCDWARF_ERRCODE(x, n0);

    sf->status = dwarf_srcfiles(die, &sf->data, &sf->size, e);
    for (Dwarf_Signed i = 0; i < attrcount ; ++i) {
      int n1 = ocdwarf_printf_worth(p, die, attrbuf[i], i, sf, e);
      if (OCDWARF_ISERRCODE(n1)) {
        ocdwarf_dealloc(p, s, attrbuf, attrcount, 0);
        return n1;
      }

      dwarf_dealloc(oc->items[OPCODE_DWARF], attrbuf[i], DW_DLA_ATTR);
      n0 += n1;
    }

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      n0 += ocdwarf_printf_srcfiles(p, sf);
    }

    dwarf_dealloc(oc->items[OPCODE_DWARF], attrbuf, DW_DLA_LIST);
  }

  return OCDWARF_ERRCODE(x, n0);
}

int ocdwarf_printf_sp(handle_t p, handle_t s, Dwarf_Die die, Dwarf_Half tag,
                  Dwarf_Bool isinfo, int level, pdwarf_srcfiles_t sf, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p) && sf) {
//    popcode_t oc = CAST(popcode_t, p);

    Dwarf_Signed cattr = 0;
    Dwarf_Attribute *pattr = 0;
    x = dwarf_attrlist(die, &pattr, &cattr, e);
    if (IS_DLV_ANY_ERROR(x)) {
      printf_e("dwarf_attrlist failed! errcode %d", x);
      return OCDWARF_ERRCODE(x, n0);
    }

    n0 += ocdwarf_printf_idx(p, level, USE_NONE);
    n0 += ocdwarf_printf_addr(p, 0xffffffff, USE_NONE);
    n0 += ocdwarf_printf_TAG(p, tag, USE_EOL);

    for (Dwarf_Signed i = 0; i < cattr; ++i) {
      int n1 = ocdwarf_printf_worth(p, die, pattr[i], i, NULL, e);
      if (OCDWARF_ISERRCODE(n1)) {
        ocdwarf_dealloc(p, s, pattr, cattr, 0);
        return n1;
      }
      n0 += n1;
    }

    ocdwarf_dealloc(p, s, pattr, cattr, 0);
    n0 += printf_eol();
  }

  return OCDWARF_ERRCODE(x, n0);
}

