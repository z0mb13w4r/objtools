#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-loc.h"

int ocdwarf_debug_loc(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n0 = 0;
  int n1 = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = ocget(p, OPCODE_THIS);

  }

  return OCDWARF_ERRCODE(x, n0);
}

