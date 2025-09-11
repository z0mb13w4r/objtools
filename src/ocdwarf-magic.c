#include <stdlib.h>
#include <dwarf.h>
#include <libdwarf.h>
#include <dwarf_base_types.h>
#include <dwarf_opaque.h>
#include <dwarf_alloc.h>
#include <dwarf_error.h>
#include <dwarf_line.h>

#include "ocdwarf-magic.h"

int dwarf_info_from_linecontext(Dwarf_Line_Context line_context,
                                Dwarf_Unsigned *section_offset, Dwarf_Unsigned *total_length,
                                Dwarf_Unsigned *prologue_length, Dwarf_Small *minimum_instruction_length, Dwarf_Ubyte *maximum_ops_per_instruction,
                                Dwarf_Bool *is_stmt, Dwarf_Sbyte *line_base, Dwarf_Small *line_range, Dwarf_Small *opcode_base,
                                Dwarf_Error *error) {
  if (!line_context || line_context->lc_magic != DW_CONTEXT_MAGIC) {
    _dwarf_error(NULL, error, DW_DLE_LINE_CONTEXT_BOTCH);
    return DW_DLV_ERROR;
  } else if (!line_context->lc_new_style_access) {
    _dwarf_error(line_context->lc_dbg, error, DW_DLE_LINE_CONTEXT_BOTCH);
    return DW_DLV_ERROR;
  }

  *section_offset =  line_context->lc_section_offset;
  *total_length = line_context->lc_total_length;

  *prologue_length = line_context->lc_prologue_length;
  *minimum_instruction_length = line_context->lc_minimum_instruction_length;
  *maximum_ops_per_instruction = line_context->lc_maximum_ops_per_instruction;

  *is_stmt = line_context->lc_default_is_stmt;
  *line_base = line_context->lc_line_base;
  *line_range = line_context->lc_line_range;
  *opcode_base = line_context->lc_opcode_base;


  return DW_DLV_OK;
}

