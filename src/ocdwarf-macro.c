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

  }

  return OCDWARF_ERRCODE(x, n0);
}

