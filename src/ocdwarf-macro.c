#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-macro.h"

static const int MAXSIZE = 23;


int ocdwarf_debug_macro(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = CAST(popcode_t, p);

    Dwarf_Unsigned number_of_ops = 232;
    Dwarf_Unsigned ops_total_byte_len = 803;

    printf_text("MacroInformationEntries count", USE_LT | USE_TAB | USE_COLON);
    printf_nice(number_of_ops, USE_DEC);
    printf_text("bytes length", USE_LT | USE_COMMA | USE_COLON);
    printf_nice(ops_total_byte_len, USE_DEC | USE_EOL);
  }

  return OCDWARF_ERRCODE(x, n0);
}

