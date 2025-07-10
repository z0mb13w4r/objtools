#ifndef __OCDWARF_PRINTF_H_
#define __OCDWARF_PRINTF_H_

#define OCDWARF_ISERRCODE(x)  ((x) < 0)
#define OCDWARF_ISNOENTRY(x)  (-1 == (x))
#define OCDWARF_ISFAILED(x)   (-2 == (x))

#define OCDWARF_ERRCODE(x,n)  (IS_DLV_OK(x) ? (n) : IS_DLV_NO_ENTRY(x) ? -1 : -2)

#include "printf.h"
#include "ocdwarf.h"

int ocdwarf_printf_me(handle_t p, const int x, const char *y, const char *z, const imode_t mode);

int ocdwarf_printf_pluck(handle_t p, const pconvert_t z, const pick_t x, const imode_t mode);

int ocdwarf_printf_AT(handle_t p, const uint64_t v, const imode_t mode);
int ocdwarf_printf_OP(handle_t p, const uint64_t v, const imode_t mode);
int ocdwarf_printf_ATE(handle_t p, const uint64_t v, const imode_t mode);
int ocdwarf_printf_CFA(handle_t p, const uint64_t v, const imode_t mode);
int ocdwarf_printf_TAG(handle_t p, const uint64_t v, const imode_t mode);
int ocdwarf_printf_EXPR(handle_t p, const uint64_t v, const imode_t mode);
int ocdwarf_printf_GNUM(handle_t p, const uint64_t v, const imode_t mode);
int ocdwarf_printf_FORM(handle_t p, const uint64_t v, const imode_t mode);
int ocdwarf_printf_LANG(handle_t p, const uint64_t v, const imode_t mode);
int ocdwarf_printf_MACRO(handle_t p, const uint64_t v, const imode_t mode);
int ocdwarf_printf_CHILDREN(handle_t p, const uint64_t v, const imode_t mode);

int ocdwarf_printf_DEC(handle_t p, const uint64_t v, const imode_t mode);
int ocdwarf_printf_HEX(handle_t p, const uint64_t v, const imode_t mode);

int ocdwarf_printf_srcfile(handle_t p, const uint32_t v, const imode_t mode);

int ocdwarf_printf_fields_description(handle_t p, const char* fields_description,
                     Dwarf_Unsigned u0, Dwarf_Unsigned u1, Dwarf_Unsigned u2,
                     Dwarf_Signed s0, Dwarf_Signed s1);

int ocdwarf_printf_value(handle_t p, Dwarf_Die die, Dwarf_Half nattr, pdwarf_srcfiles_t sf, Dwarf_Error *e);
int ocdwarf_printf_merit(handle_t p, Dwarf_Die die, Dwarf_Attribute attr, Dwarf_Half nattr, Dwarf_Error *e);
int ocdwarf_printf_worth(handle_t p, Dwarf_Die die, Dwarf_Attribute attr, Dwarf_Signed index, Dwarf_Error *e);

int ocdwarf_printf_names(handle_t p, Dwarf_Die die, Dwarf_Error *e);

int ocdwarf_printf_cu(handle_t p, Dwarf_Die die, Dwarf_Half tag,
              Dwarf_Bool isinfo, int level, Dwarf_Error *e);

int ocdwarf_printf_sp(handle_t p, Dwarf_Die die, Dwarf_Half tag,
              Dwarf_Bool isinfo, int level, Dwarf_Error *e);

int ocdwarf_printf(handle_t p, handle_t s, Dwarf_Die die, Dwarf_Bool isinfo,
                     int level, Dwarf_Error *e);

#endif

