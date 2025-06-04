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
  int res = DW_DLV_OK;

  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
  }

  return res;
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

