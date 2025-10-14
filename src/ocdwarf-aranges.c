#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-aranges.h"

int ocdwarf_debug_aranges(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    const int MAXSIZE = ocis64(p) ? 17 : 9;
    const imode_t USE_LHEXNN = ocis64(p) ? USE_LHEX64 : USE_LHEX32;

    if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      n += ocdwarf_printf_groups(p, ocget(p, OPCODE_DWARF_ERROR));
    } else {
      n += printf_text("Address", USE_LT | SET_PAD(MAXSIZE));
      n += printf_text("Length", USE_LT);
      n += printf_eol();
    }

    Dwarf_Signed arange_count = 0;
    Dwarf_Arange *arange_array = NULL;
    x = dwarf_get_aranges(ocget(p, OPCODE_DWARF_DEBUG), &arange_array, &arange_count, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_ANY_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
      if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_get_aranges failed! - %d", x);
      }
#endif
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
        if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
          n += ocdwarf_printf(p, cu_die, isinfo, i, ocget(p, OPCODE_DWARF_ERROR));

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
        } else {
          n += printf_nice(start,  USE_LHEXNN | USE_NOSPACE);
          n += printf_nice(length, USE_LHEXNN);
        }

        n += printf_eol();
      } else if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
        n += printf_text("arange end", USE_LT);
      } else {
        n += printf_nice(0, USE_LHEXNN | USE_NOSPACE);
        n += printf_nice(0, USE_LHEXNN);
      }

      ocdwarf_dealloc(p, cu_die, DW_DLA_DIE);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

