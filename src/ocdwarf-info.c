#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-info.h"

int ocdwarf_die_and_siblings(handle_t p, Dwarf_Die die,
                  Dwarf_Bool isinfo, int level, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    n += ocdwarf_printf(p, die, isinfo, level, e);

    if ((0 == level) && MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      n += printf_text("LOCAL_SYMBOLS", USE_LT | USE_COLON | USE_EOL);
    }

    Dwarf_Die cur_die = die;

    for ( ; ; ) {
      Dwarf_Die child = 0;
      x = dwarf_child(cur_die, &child, e);
      if (IS_DLV_ERROR(x)) {
        dwarf_dealloc_die(cur_die);
        ocdwarf_finish(p, e);
        printf_x("dwarf_child, level %d", level);
      } else if (IS_DLV_OK(x)) {
        n += ocdwarf_die_and_siblings(p, child, isinfo, level + 1, e);
        dwarf_dealloc_die(child);
        child = 0;
      }
      /* x == DW_DLV_NO_ENTRY or DW_DLV_OK */
      Dwarf_Die sib_die = 0;
      x = dwarf_siblingof_b(ocget(p, OPCODE_DWARF_DEBUG), cur_die, isinfo, &sib_die, e);
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ERROR(x)) {
        ocdwarf_finish(p, e);
        printf_x("dwarf_siblingof_b, level %d", level);
      }
      if (cur_die != die) {
        dwarf_dealloc_die(cur_die);
      }
      cur_die = sib_die;
      n += ocdwarf_printf(p, cur_die, isinfo, level, e);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_debug_info(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n0 = 0;
  int n1 = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    Dwarf_Bool isinfo = TRUE; /* our data is not DWARF4 .debug_types. */
    int        level = 0;

    if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      n0 += ocdwarf_printf_groups(p, ocget(p, OPCODE_DWARF_ERROR));
    }

    for ( ; ; ) {
      Dwarf_Die no_die = 0;
      Dwarf_Die cu_die = 0;

      n1 = ocdwarf_next_cu_header(p, &cu_die, ocget(p, OPCODE_DWARF_ERROR));
      if (ECODE_ISNOENTRY(n1)) return n0;
      else if (ECODE_ISFAILED(n1)) {
        ocdwarf_dealloc_error(p, NULL);
        return n1;
      }

      n0 += n1;

      x = dwarf_siblingof_b(ocget(p, OPCODE_DWARF_DEBUG), no_die, isinfo, &cu_die, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_NO_ENTRY(x)) return n0;
      else if (IS_DLV_ERROR(x)) {
        ocdwarf_dealloc_error(p, NULL);
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("dwarf_siblingof_b failed, no CU die");
#endif
        return OCDWARF_ERRCODE(x, n0);
      }

      n1 = ocdwarf_die_and_siblings(p, cu_die, isinfo, level, ocget(p, OPCODE_DWARF_ERROR));
      if (ECODE_ISFAILED(n1)) {
        dwarf_dealloc_die(cu_die);
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("ocdwarf_die_and_siblings failed! %d", n1);
#endif
        return n1;
      }

      n0 += n1;

      dwarf_dealloc_die(cu_die);
      ocdwarf_sfreset(p);
    }
  }

  return OCDWARF_ERRCODE(x, n0);
}

