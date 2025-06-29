#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-abbrev.h"

static const int MAXSIZE = 23;

int ocdwarf_debug_abbrev(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = CAST(popcode_t, p);

    Dwarf_Abbrev   abbrev = 0;
    Dwarf_Unsigned offset = 0;
    Dwarf_Unsigned length = 0;
    Dwarf_Unsigned unused_entry_count = 0;

    x = dwarf_get_abbrev(ocget(p, OPCODE_DWARF_DEBUG), offset, &abbrev, &length, &unused_entry_count, ocget(p, OPCODE_DWARF_ERROR));

    ocdwarf_dealloc(p, abbrev, DW_DLA_ABBREV);
  }

  return OCDWARF_ERRCODE(x, n0);
}

