#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-eh-frame.h"

static const int MAXSIZE = 23;

int ocdwarf_eh_frame(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = CAST(popcode_t, p);

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


    ocdwarf_dealloc_fde_cie_list(p, cie_data, cie_element_count, fde_data, fde_element_count);
  }

  return OCDWARF_ERRCODE(x, n);
}

