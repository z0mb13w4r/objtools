#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-abbrev.h"

static const int MAXSIZE = 23;

int ocdwarf_abbrev_cu(handle_t p, Dwarf_Unsigned offset, Dwarf_Unsigned nabbrev, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    Dwarf_Unsigned length = 0;
    Dwarf_Abbrev   abbrev = 0;
    Dwarf_Unsigned abbrev_entry_count = 0;
    x = dwarf_get_abbrev(ocget(p, OPCODE_DWARF_DEBUG), offset, &abbrev, &length, &abbrev_entry_count, e);
    if (IS_DLV_NO_ENTRY(x)) return n;
    else if (IS_DLV_ERROR(x)) {
      printf_e("dwarf_get_abbrev failed! - %d", x);
      return OCDWARF_ERRCODE(x, n);
    }

    n += printf_nice(nabbrev, USE_DEC2 | USE_TB);
    n += printf_nice(offset,  USE_FHEX32 | USE_TB | USE_NOSPACE);

    Dwarf_Unsigned abbrev_code = 0;
    x = dwarf_get_abbrev_code(abbrev, &abbrev_code, e);
    if (IS_DLV_OK(x)) {
      n += printf_text("code", USE_LT | USE_TBLT | USE_COLON);
      n += printf_nice(abbrev_code, USE_DEC | USE_TBRT);
    }

    Dwarf_Half abbrev_tag = 0;
    x = dwarf_get_abbrev_tag(abbrev, &abbrev_tag, e);
    if (IS_DLV_OK(x)) {
      n += ocdwarf_printf_TAG(p, abbrev_tag, USE_NONE);
    }

    Dwarf_Signed child_flag = 0;
    x = dwarf_get_abbrev_children_flag(abbrev, &child_flag, e);
    if (IS_DLV_OK(x)) {
      n += ocdwarf_printf_CHILDREN(p, child_flag, USE_NONE);
    }

    n += printf_eol();

    ocdwarf_dealloc(p, abbrev, DW_DLA_ABBREV);
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_debug_abbrev(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    Dwarf_Abbrev   abbrev = 0;
    Dwarf_Unsigned offset = 0;
    Dwarf_Unsigned length = 0;
    Dwarf_Unsigned unused_entry_count = 0;

    x = dwarf_get_abbrev(ocget(p, OPCODE_DWARF_DEBUG), offset, &abbrev, &length, &unused_entry_count, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_NO_ENTRY(x)) return n;
    else if (IS_DLV_ERROR(x)) {
      printf_e("dwarf_get_abbrev failed! - %d", x);
      return OCDWARF_ERRCODE(x, n);
    }

    Dwarf_Unsigned nabbrev = 1;
    n += ocdwarf_abbrev_cu(p, offset, nabbrev, ocget(p, OPCODE_DWARF_ERROR));

    ocdwarf_dealloc(p, abbrev, DW_DLA_ABBREV);
  }

  return OCDWARF_ERRCODE(x, n);
}

