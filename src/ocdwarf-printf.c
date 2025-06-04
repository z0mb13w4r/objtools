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

