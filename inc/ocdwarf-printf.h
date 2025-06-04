#ifndef __OCDWARF_PRINTF_H_
#define __OCDWARF_PRINTF_H_

#include "printf.h"
#include "ocdwarf.h"

typedef struct dwarf_srcfiles_s {
  char       **data;
  Dwarf_Signed size;
  int          status;
} dwarf_srcfiles_t;

int ocdwarf_printf_srcfiles(handle_t p, handle_t s, dwarf_srcfiles_t *sf);

#endif

