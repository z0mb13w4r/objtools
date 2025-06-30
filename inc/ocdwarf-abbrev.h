#ifndef __OCDWARF_ABBREV_H_
#define __OCDWARF_ABBREV_H_

#include "ocdwarf.h"
#include "ocdwarf-printf.h"

int ocdwarf_abbrev_one(handle_t p, Dwarf_Unsigned offset, Dwarf_Unsigned nabbrev,
                     Dwarf_Unsigned *size, Dwarf_Error *e);
int ocdwarf_debug_abbrev(handle_t p, handle_t s, handle_t d);

#endif

