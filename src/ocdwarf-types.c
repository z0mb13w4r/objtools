#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-types.h"

int ocdwarf_debug_types(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {

  }

  return OCDWARF_ERRCODE(x, n);
}

