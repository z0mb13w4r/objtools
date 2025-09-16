#include "ocdwarf-engine.h"

static int execute_next_cu_header(handle_t p, Dwarf_Die *cu_die, Dwarf_Error *e) {
  if (isopcode(p) && cu_die) {
    pocdwarf_t ws = ocget(p, OPCODE_DWARF);

    Dwarf_Bool     isinfo = TRUE; /* our data is not DWARF4 .debug_types. */
    Dwarf_Unsigned next_cu_header_offset = 0;
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Unsigned type_offset    = 0;
    Dwarf_Half     extension_size = 0;
    Dwarf_Half     header_cu_type = 0;
    Dwarf_Half     address_size  = 0;
    Dwarf_Sig8     type_signature = ZEROSIGNATURE;
    Dwarf_Off      abbrev_offset = 0;

    return dwarf_next_cu_header_e(ocget(p, OPCODE_DWARF_DEBUG), isinfo, cu_die, &cu_header_length, &ws->cu_version_stamp,
                     &abbrev_offset, &address_size, &ws->cu_offset_size, &extension_size, &type_signature, &type_offset,
                     &next_cu_header_offset, &header_cu_type, e);
  }

  return DW_DLV_ERROR;
}

static handle_t execute_info(handle_t p, handle_t q, Dwarf_Error *e) {
  if (isopcode(p) && isocengine(q)) {
    Dwarf_Bool isinfo = TRUE; /* our data is not DWARF4 .debug_types. */

    for ( ; ; ) {
      Dwarf_Die no_die = 0;
      Dwarf_Die cu_die = 0;

      int x = execute_next_cu_header(p, &cu_die, e);
      if (IS_DLV_OK(x)) {
        x = dwarf_siblingof_b(ocget(p, OPCODE_DWARF_DEBUG), no_die, isinfo, &cu_die, e);
      }


      if (IS_DLV_ANY_ERROR(x)) {
        if (IS_DLV_ERROR(x)) {
          ocdwarf_dealloc_error(p, NULL);
        }

        return e;
      }



      dwarf_dealloc_die(cu_die);
      ocdwarf_sfreset(p);
    }

    return e;
  }

  return NULL;
}

handle_t ocdwarf_create(handle_t p, handle_t e) {
  return execute_info(p, e, ocget(p, OPCODE_DWARF_ERROR));
}

