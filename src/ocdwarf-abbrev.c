#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-abbrev.h"

int ocdwarf_abbrev_one(handle_t p, Dwarf_Unsigned offset, Dwarf_Unsigned nabbrev, Dwarf_Unsigned *size, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && size) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    *size = 0;

    Dwarf_Abbrev   abbrev = 0;
    Dwarf_Unsigned abbrev_entry_count = 0;
    x = dwarf_get_abbrev(ocget(p, OPCODE_DWARF_DEBUG), offset, &abbrev, size, &abbrev_entry_count, e);
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_get_abbrev failed! - %d", x);
      }
      return OCDWARF_ERRCODE(x, n);
    }

    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n += ocdwarf_printf_DEC(p, nabbrev, USE_NOSPACE);
      n += ocdwarf_printf_HEX(p, offset, USE_NOSPACE);

      Dwarf_Unsigned abbrev_code = 0;
      x = dwarf_get_abbrev_code(abbrev, &abbrev_code, e);
      if (IS_DLV_OK(x)) {
        n += printf_text("code", USE_LT | USE_TBLT | USE_COLON);
        n += printf_nice(abbrev_code, USE_DEC | USE_TBRT);
      }
    }

    Dwarf_Half abbrev_tag = 0;
    x = dwarf_get_abbrev_tag(abbrev, &abbrev_tag, e);
    if (IS_DLV_OK(x)) {
      if (0 == abbrev_tag) {
        if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
          n += printf_text("Abbrev 0: null abbrev entry", USE_LT | USE_SPACE);
        }
      } else {
        n += printf_nice(nabbrev, USE_DEC | USE_NOSPACE);
        n += ocdwarf_printf_TAG(p, abbrev_tag, USE_NONE);
      }
    }

    if (0 != abbrev_tag) {
      Dwarf_Signed child_flag = 0;
      x = dwarf_get_abbrev_children_flag(abbrev, &child_flag, e);
      if (IS_DLV_OK(x)) {
        n += ocdwarf_printf_CHILDREN(p, child_flag, USE_NONE);
      }
    }

    n += printf_eol();

    if (abbrev_entry_count < 1) {
      if (0 != abbrev_tag && MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        n += printf_text("This abbreviation code has no entries", USE_LT | USE_TAB | USE_EOL);
      }
      if (*size == 0 || *size == 1) {
        n += printf_eol();
      }
    } else {
      for (Dwarf_Unsigned i = 0; i < abbrev_entry_count; ++i) {
        Dwarf_Unsigned nattr = 0;
        Dwarf_Unsigned nform = 0;
        Dwarf_Signed   impl_const = 0;
        Dwarf_Off      offset2 = 0;
        Dwarf_Bool filter_outliers = FALSE;
        x = dwarf_get_abbrev_entry_b(abbrev, i, filter_outliers, &nattr, &nform, &impl_const, &offset2, e);
        if (IS_DLV_OK(x)) {
          if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
            n += ocdwarf_printf_HEX(p, offset2, USE_TAB);
          }
          n += ocdwarf_printf_AT(p, nattr, USE_NONE);
          n += ocdwarf_printf_FORM(p, nform, USE_NONE);
          n += printf_eol();
        }
      }
    }

    ocdwarf_dealloc(p, abbrev, DW_DLA_ABBREV);
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_debug_abbrev(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n0 += ocdwarf_printf_groups(p, ocget(p, OPCODE_DWARF_ERROR));
    }

    Dwarf_Unsigned offset = 0;
    Dwarf_Unsigned nabbrev = 1;

    for ( ; ; ) {
      Dwarf_Unsigned size = 0;
      int n1 = ocdwarf_abbrev_one(p, offset, nabbrev, &size, ocget(p, OPCODE_DWARF_ERROR));
      if (ECODE_ISEVIL(n1) || 1 == size || 0 == size) break;
      nabbrev += 1;
      offset += size;
      n0 += n1;
    }
  }

  return OCDWARF_ERRCODE(x, n0);
}

