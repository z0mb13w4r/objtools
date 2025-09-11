#include <stdlib.h>
#include <dwarf.h>
#include <libdwarf.h>
#include <dwarf_base_types.h>
#include <dwarf_opaque.h>
#include <dwarf_alloc.h>
#include <dwarf_error.h>
#include <dwarf_line.h>

#include "ocdwarf-magic.h"

int
dwarf_info_from_linecontext(Dwarf_Line_Context line_context,
    Dwarf_Error *error) {
  if (!line_context || line_context->lc_magic != DW_CONTEXT_MAGIC) {
    _dwarf_error(NULL, error, DW_DLE_LINE_CONTEXT_BOTCH);
    return DW_DLV_ERROR;
  } else if (!line_context->lc_new_style_access) {
    _dwarf_error(line_context->lc_dbg, error, DW_DLE_LINE_CONTEXT_BOTCH);
    return DW_DLV_ERROR;
  }

  return DW_DLV_OK;
}

