#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-aranges.h"

int ocdwarf_debug_aranges(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    Dwarf_Signed arange_count = 0;
    Dwarf_Arange *arange_array = NULL;
    x = dwarf_get_aranges(ocget(p, OPCODE_DWARF_DEBUG), &arange_array, &arange_count, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_get_aranges failed! - %d", x);
      }
      return OCDWARF_ERRCODE(x, n);
    }

    for (Dwarf_Signed i = 0; i < arange_count; ++i) {
      Dwarf_Unsigned segment = 0;
      Dwarf_Unsigned segment_entry_size = 0;
      Dwarf_Addr start = 0;
      Dwarf_Unsigned length = 0;
      Dwarf_Off cu_die_offset = 0;
      x = dwarf_get_arange_info_b(arange_array[i], &segment, &segment_entry_size,
                     &start, &length, &cu_die_offset, ocget(p, OPCODE_DWARF_ERROR));

      Dwarf_Die cu_die = NULL;
      Dwarf_Bool isinfo = TRUE;
      x = dwarf_offdie_b(ocget(p, OPCODE_DWARF_DEBUG), cu_die_offset, isinfo, &cu_die, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_OK(x) && (0 != start || 0 != length)) {
        n += ocdwarf_printf(p, s, cu_die, isinfo, i, ocget(p, OPCODE_DWARF_ERROR));

        if (segment_entry_size) {
          n += printf_text("arange starts at segment", USE_LT);
          n += printf_nice(segment, USE_FHEX32);
          n += printf_text(", offset", USE_LT);
        } else {
          n += printf_text("arange starts at", USE_LT);
        }

        n += printf_nice(start, USE_FHEX32);
        n += printf_text("length of", USE_LT | USE_COMMA);
        n += printf_nice(length, USE_FHEX32);
        n += printf_text("cu_die_offset =", USE_LT | USE_COMMA);
        n += printf_nice(cu_die_offset, USE_FHEX32);
        n += printf_eol();
      } else {
        n += printf_text("arange end", USE_LT);
      }

      ocdwarf_dealloc(p, cu_die, DW_DLA_DIE);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

