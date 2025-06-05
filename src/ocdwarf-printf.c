#include "options.h"
#include "ocdwarf-printf.h"

int ocdwarf_printf_srcfiles(handle_t p, handle_t s, dwarf_srcfiles_t *sf) {
  int n = 0;
  if (isopcode(p)) {
    if (IS_DLV_OK(sf->status) && 0 != sf->size) {
      for (Dwarf_Signed i = 0; i < sf->size; ++i) {
        n += printf_nice(i, USE_DEC3 | USE_TB | USE_COLON);
        n += printf_text(sf->data[i], USE_LT | USE_EOL);
      }
    }
  }

  return n;
}

Dwarf_Addr low_pc_addr = 0;

int ocdwarf_printf_cu_attr(handle_t p, handle_t s, Dwarf_Signed attridx, Dwarf_Attribute attr, Dwarf_Error *e) {
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);

    int res = 0;

    Dwarf_Half formnum = 0;
    res = dwarf_whatform(attr, &formnum, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_whatform failed! res %d", res);
      return res;
    }

    Dwarf_Half attrnum = 0;
    res = dwarf_whatattr(attr, &attrnum, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_whatattr failed! res %d", res);
      return res;
    }

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      printf_nice(attridx, USE_DEC2 | USE_SB | USE_TAB);
      printf_nice(attrnum, USE_FHEX16);
    } else {
      printf_pack(8);
    }

    printf_pick(zDWAT, attrnum, USE_SPACE | SET_PAD(18));

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      printf_nice(formnum, USE_FHEX8);
      printf_pick(zDWFORM, formnum, USE_SPACE | SET_PAD(22));
    }

    if (isused(zFORMSTRING, formnum)) {
      char *str = NULL;
      res = dwarf_formstring(attr, &str, e);
      if (res != DW_DLV_OK) {
        printf_e("dwarf_formstring failed! res %d", res);
        return res;
      }

      printf_text(str, USE_LT | USE_SPACE);
    } else if (isused(zFORMUDATA, formnum)) {
      Dwarf_Unsigned value = 0;
      res = dwarf_formudata(attr, &value, e);
      if (res != DW_DLV_OK) {
        printf_e("dwarf_formudata failed! res %d", res);
        return res;
      }

      if (DW_AT_high_pc == attrnum) {
        printf_text("offset-from-lowpc", USE_LT | USE_SPACE | USE_TB);
        printf_nice(value, USE_DEC);
        printf_text("highpc", USE_LT | USE_SPACE | USE_TBLT | USE_COLON);
        printf_nice(low_pc_addr + value, USE_FHEX32 | USE_TBRT);
      } else if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
        printf_nice(value, USE_FHEX16);
      }

      if (DW_AT_language == attrnum) {
        printf_pick(zDWLANG, value, USE_SPACE);
      }
    } else if (isused(zFORMADDR, formnum)) {
      Dwarf_Addr addr = 0;
      res = dwarf_formaddr(attr, &addr, e);
      if (res != DW_DLV_OK) {
        printf_e("dwarf_formaddr failed! res %d", res);
        return res;
      }

      if (DW_AT_low_pc == attrnum) {
        low_pc_addr = addr;
      }

      printf_nice(addr, USE_FHEX32);
    } else if (isused(zFORMGREF, formnum)) {
      Dwarf_Unsigned value = 0;
      res = dwarf_global_formref(attr, &value, e);
      if (res != DW_DLV_OK) {
        printf_e("dwarf_global_formref failed! res %d", res);
        return res;
      }

      printf_nice(value, USE_FHEX32);
    }

    printf_eol();

    return DW_DLV_OK;
  }

  return DW_DLV_ERROR;
}

static int ocdwarf_printf_name(handle_t p, handle_t s, Dwarf_Die die, Dwarf_Attribute attr, Dwarf_Error *e) {
  int x = DW_DLV_OK;
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
  int x = DW_DLV_OK;
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
  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    int res = DW_DLV_OK;

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      char *name = 0;
      res = dwarf_diename(die, &name, e);
      if (res == DW_DLV_ERROR) {
        printf_e("dwarf_diename, level %d", level);
        return res;
      }

      printf_text(name, USE_LT | USE_EOL);
    }

    Dwarf_Half tag = 0;
    res = dwarf_tag(die, &tag, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_tag failed! res %d", res);
      return res;
    }

    Dwarf_Off overall_offset = 0;
    res = dwarf_dieoffset(die, &overall_offset, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_dieoffset failed! res %d", res);
      return res;
    }

    Dwarf_Off offset = 0;
    res = dwarf_die_CU_offset(die, &offset, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_die_CU_offset failed! res %d", res);
      return res;
    }

    printf_text("COMPILE_UNIT<header overall offset =", USE_LT);
    printf_nice(overall_offset - offset, USE_FHEX32 | USE_TBRT | USE_COLON | USE_EOL);

    printf_nice(level, USE_DEC2 | USE_TB);
    printf_nice(0xffffffff, USE_FHEX32 | USE_TB);
    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      printf_nice(tag, USE_FHEX16);
    }
    printf_pick(zDWTAG, tag, USE_SPACE | USE_EOL);

    Dwarf_Signed attrcount = 0;
    Dwarf_Attribute *attrbuf = 0;
    res = dwarf_attrlist(die, &attrbuf, &attrcount, e);
    if (res != DW_DLV_OK) {
      printf_e("dwarf_attrlist failed! res %d", res);
      return res;
    }

    for (Dwarf_Signed i = 0; i < attrcount; ++i) {
      res = ocdwarf_printf_cu_attr(p, s, i, attrbuf[i], e);
      if (res != DW_DLV_OK) {
        ocdwarf_dealloc(p, s, attrbuf, attrcount, 0);
        printf("ocdwarf_printf_cu_attr failed! res %d", res);
        return res;
      }
    }

    ocdwarf_dealloc(p, s, attrbuf, attrcount, 0);
    return DW_DLV_OK;
  }

  return DW_DLV_ERROR;
}

