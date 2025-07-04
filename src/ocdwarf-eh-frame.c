#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-eh-frame.h"

static const int MAXSIZE = 23;

int ocdwarf_eh_frame(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    Dwarf_Cie *cie_data = NULL;
    Dwarf_Fde *fde_data = NULL;
    Dwarf_Signed cie_element_count = 0;
    Dwarf_Signed fde_element_count = 0;
    x = dwarf_get_fde_list_eh(ocget(p, OPCODE_DWARF_DEBUG), &cie_data, &cie_element_count,
                     &fde_data, &fde_element_count, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_NO_ENTRY(x)) return n;
    else if (IS_DLV_ERROR(x)) {
      printf_e("dwarf_get_fde_list_eh failed! - %d", x);
      return OCDWARF_ERRCODE(x, n);
    }

    const char *sec_name = 0;
    x = dwarf_get_frame_section_name_eh_gnu(ocget(p, OPCODE_DWARF_DEBUG), &sec_name, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_ANY_ERROR(x) ||  0 == sec_name || 0 == *sec_name) {
      sec_name = ".eh_frame";
    }

    if (IS_DLV_ERROR(x)) {
      ocdwarf_dealloc_error(p, NULL);
    }

    for (Dwarf_Signed i = 0; i < fde_element_count; ++i) {
      Dwarf_Addr     low_pc = 0;
      Dwarf_Unsigned func_length = 0;
      Dwarf_Small   *fde_bytes = NULL;
      Dwarf_Unsigned fde_bytes_length = 0;
      Dwarf_Off      fde_offset = 0;
      Dwarf_Off      cie_offset = 0;
      Dwarf_Signed   cie_index = 0;

      Dwarf_Fde fde = fde_data[i];
      x = dwarf_get_fde_range(fde, &low_pc, &func_length, &fde_bytes, &fde_bytes_length,
                     &cie_offset, &cie_index, &fde_offset, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_get_fde_range failed! - %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      n += ocdwarf_printf_DEC(p, i, USE_NONE);

      // <0x00001020:0x000010c0>
      n += printf_nice(low_pc, USE_FHEX32 | USE_TBLT | USE_COLON | USE_NOSPACE);
      n += printf_nice(low_pc + func_length, USE_FHEX32 | USE_TBRT | USE_NOSPACE);

      // <cie offset 0x00000034::cie index 0>
      n += printf_text("cie offset", USE_LT | USE_TBLT);
      n += printf_nice(cie_offset, USE_FHEX32);
      n += printf_text("::cie index", USE_LT);
      n += printf_nice(cie_index, USE_DEC | USE_TBRT);

      // <fde offset 0x00000030::length 0x00000024>
      n += printf_text("fde offset", USE_LT | USE_TBLT);
      n += printf_nice(fde_offset, USE_FHEX32);
      n += printf_text("::length", USE_LT);
      n += printf_nice(fde_bytes_length, USE_FHEX32 | USE_TBRT);
      n += printf_eol();
    }

    ocdwarf_dealloc_fde_cie_list(p, cie_data, cie_element_count, fde_data, fde_element_count);
  }

  return OCDWARF_ERRCODE(x, n);
}

