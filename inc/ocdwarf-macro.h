#ifndef __OCDWARF_MACRO_H_
#define __OCDWARF_MACRO_H_

#include "ocdwarf.h"
#include "ocdwarf-printf.h"

int ocdwarf_debug_macro_ops(handle_t p, Dwarf_Macro_Context context, Dwarf_Unsigned macro_unit_offset,
                     Dwarf_Unsigned count, Dwarf_Error *e);

int ocdwarf_debug_macro(handle_t p, handle_t s, handle_t d);

#endif

