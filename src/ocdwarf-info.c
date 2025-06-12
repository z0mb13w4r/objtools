#include "printf.h"
#include "externs.h"
#include "ocdwarf-info.h"

int ocdwarf_die_and_siblings(handle_t p, handle_t s, Dwarf_Die die,
                  Dwarf_Bool isinfo, int level, pdwarf_srcfiles_t sf, Dwarf_Error *e) {
  Dwarf_Die cur_die = die;
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    n += ocdwarf_printf(p, s, die, isinfo, level, sf, e);

    for ( ; ; ) {
      Dwarf_Die child = 0;
      x = dwarf_child(cur_die, &child, e);
      if (IS_DLV_ERROR(x)) {
        dwarf_dealloc_die(cur_die);
        if (e) {
          dwarf_dealloc_error(oc->items[OPCODE_DWARF], *e);
        }
        dwarf_finish(oc->items[OPCODE_DWARF]);
        printf_x("dwarf_child, level %d", level);
      } else if (IS_DLV_OK(x)) {
        n += ocdwarf_die_and_siblings(p, s, child, isinfo, level + 1, sf, e);
        dwarf_dealloc_die(child);
        child = 0;
      }
      /* x == DW_DLV_NO_ENTRY or DW_DLV_OK */
      Dwarf_Die sib_die = 0;
      x = dwarf_siblingof_b(oc->items[OPCODE_DWARF], cur_die, isinfo, &sib_die, e);
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ERROR(x)) {
        if (e) {
          printf_e("errcode = %d, errmessage = %s", dwarf_errmsg(*e));
        }
        dwarf_finish(oc->items[OPCODE_DWARF]);
        printf_x("dwarf_siblingof_b, level %d", level);
      }
      if (cur_die != die) {
        dwarf_dealloc_die(cur_die);
      }
      cur_die = sib_die;
      n += ocdwarf_printf(p, s, cur_die, isinfo, level, sf, e);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_debug_info(handle_t p, handle_t s, handle_t d) {
  int n = 0;
//  printf_text("Compilation Unit @ offset 0x0", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
//  printf_text("Length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//  printf_text("Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//  printf_text("Abbrev Offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//  printf_text("Pointer Size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);

  return n;
}

