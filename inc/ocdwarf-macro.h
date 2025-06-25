#ifndef __OCDWARF_MACRO_H_
#define __OCDWARF_MACRO_H_

#include "ocdwarf.h"
#include "ocdwarf-printf.h"

int ocdwarf_debug_macro_ops(handle_t p, Dwarf_Die die, Dwarf_Macro_Context context, int level,
                     Dwarf_Half macro_version, Dwarf_Unsigned macro_unit_offset,
                     Dwarf_Unsigned count, Dwarf_Error *e);
int ocdwarf_debug_macro_context(handle_t p, Dwarf_Die die, Dwarf_Macro_Context context, int level,
                     Dwarf_Unsigned macro_unit_offset, Dwarf_Unsigned number_of_ops,
                     Dwarf_Unsigned ops_total_byte_len, Dwarf_Error *e);
int ocdwarf_debug_macro_offset(handle_t p, Dwarf_Die die, int level,
                     Dwarf_Unsigned macro_offset, Dwarf_Error *e);

int ocdwarf_debug_macro(handle_t p, handle_t s, handle_t d);

#endif

