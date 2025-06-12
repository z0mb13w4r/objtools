#ifndef __OCDWARF_INFO_H_
#define __OCDWARF_INFO_H_

#include "ocdwarf.h"
#include "ocdwarf-printf.h"

int ocdwarf_die_and_siblings(handle_t p, handle_t s, Dwarf_Die die,
                  Dwarf_Bool isinfo, int level, pdwarf_srcfiles_t sf, Dwarf_Error *e);

int ocdwarf_debug_info(handle_t p, handle_t s, handle_t d);

#endif

