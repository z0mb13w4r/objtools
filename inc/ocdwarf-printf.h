#ifndef __OCDWARF_PRINTF_H_
#define __OCDWARF_PRINTF_H_

#define OCDWARF_ERRCODE(x,n)  (IS_DLV_OK(x) ? (n) : IS_DLV_NO_ENTRY(x) ? -1 : -2)

#include "printf.h"
#include "ocdwarf.h"

int ocdwarf_printf_names(handle_t p, handle_t s, Dwarf_Die die, Dwarf_Error *e);
int ocdwarf_printf_srcfiles(handle_t p, handle_t s, dwarf_srcfiles_t *sf);
int ocdwarf_printf_one(handle_t p, handle_t s, Dwarf_Die die, int level, Dwarf_Error *e);
int ocdwarf_printf_cu_attr(handle_t p, handle_t s, Dwarf_Signed attridx, Dwarf_Attribute attr, Dwarf_Error *e);

#endif

