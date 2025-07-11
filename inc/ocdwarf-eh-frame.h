#ifndef __OCDWARF_EH_FRAME_H_
#define __OCDWARF_EH_FRAME_H_

#include "ocdwarf.h"
#include "ocdwarf-printf.h"

int ocdwarf_eh_frame_cies(handle_t p, Dwarf_Die die, Dwarf_Cie *cie_data, Dwarf_Signed cie_element_count, Dwarf_Error *e);
int ocdwarf_eh_frame_fdes(handle_t p, Dwarf_Fde *fde_data, Dwarf_Signed fde_element_count, Dwarf_Error *e);

int ocdwarf_eh_frame(handle_t p, handle_t s, handle_t d);

#endif

