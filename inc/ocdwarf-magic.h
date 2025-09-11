#ifndef __OCDWARF_MAGIC_H_
#define __OCDWARF_MAGIC_H_

#include "ocdwarf.h"

int dwarf_info_from_linecontext(Dwarf_Line_Context line_context,
                                Dwarf_Unsigned *section_offset,
                                Dwarf_Unsigned *prologue_length, Dwarf_Small *minimum_instruction_length, Dwarf_Ubyte *maximum_ops_per_instruction,
                                Dwarf_Bool *is_stmt, Dwarf_Sbyte *line_base, Dwarf_Small *line_range, Dwarf_Small *opcode_base,
                                Dwarf_Error *error);

#endif

