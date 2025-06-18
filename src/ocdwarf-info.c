#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-info.h"

static const int MAXSIZE = 23;

static const Dwarf_Sig8 zerosignature;

int ocdwarf_die_and_siblings(handle_t p, handle_t s, Dwarf_Die die,
                  Dwarf_Bool isinfo, int level, pdwarf_srcfiles_t sf, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = CAST(popcode_t, p);
    n += ocdwarf_printf(p, s, die, isinfo, level, sf, e);

    if (0 == level) {
      n += printf_text("LOCAL_SYMBOLS", USE_LT | USE_COLON | USE_EOL);
    }

    Dwarf_Die cur_die = die;

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
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = CAST(popcode_t, p);

    Dwarf_Bool isinfo = TRUE; /* our data is not DWARF4 .debug_types. */
    Dwarf_Unsigned next_cu_header_offset = 0;
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Unsigned type_offset    = 0;
    Dwarf_Half     extension_size = 0;
    Dwarf_Half     header_cu_type = 0;
    Dwarf_Half     address_size  = 0;
    Dwarf_Sig8     type_signature = zerosignature;
    Dwarf_Off      abbrev_offset = 0;
    int            level = 0;

    for ( ; ; ) {
      Dwarf_Die no_die = 0;
      Dwarf_Die cu_die = 0;

      MALLOCS(dwarf_srcfiles_t, sf);
      sf.status = DW_DLV_ERROR;

      x = dwarf_next_cu_header_d(oc->items[OPCODE_DWARF], isinfo, &cu_header_length, &cu_version_stamp,
                  &abbrev_offset, &address_size, &cu_offset_size, &extension_size, &type_signature, &type_offset,
                  &next_cu_header_offset, &header_cu_type, oc->items[OPCODE_DWARF_ERROR]);
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ANY_ERROR(x)) {
        if (IS_DLV_ERROR(x)) {
          printf_e("dwarf errmsg: %s", dwarf_errmsg(*ocgetdwarferr(p)));
        }
        printf_x("Next cu header result %d, line %d", x, __LINE__);
      }

      if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
        printf_text("CU header length", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(cu_header_length, USE_FHEX | USE_EOL);
        printf_text("Version stamp", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(cu_version_stamp, USE_DEC | USE_EOL);
        printf_text("Address size", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(address_size, USE_DEC | USE_EOL);
        printf_text("Type offset", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(type_offset, USE_FHEX | USE_EOL);
        printf_text("Extension size", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(extension_size, USE_DEC | USE_EOL);
        printf_text("CU offset size", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(cu_offset_size, USE_DEC | USE_EOL);
        printf_text("CU next header offset", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(next_cu_header_offset, USE_FHEX | USE_EOL);
        printf_text("CU header type", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(header_cu_type, USE_FHEX | USE_EOL);
      }

      x = dwarf_siblingof_b(oc->items[OPCODE_DWARF], no_die, isinfo, &cu_die, oc->items[OPCODE_DWARF_ERROR]);
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ERROR(x)) {
        printf_e("dwarf errmsg: %s", dwarf_errmsg(*ocgetdwarferr(p)));
        printf_e("dwarf_siblingof_b failed, no CU die");
        return OCDWARF_ERRCODE(x, n0);
      }

      int n1 = ocdwarf_die_and_siblings(p, s, cu_die, isinfo, level, &sf, oc->items[OPCODE_DWARF_ERROR]);
      if (OCDWARF_ISFAILED(n1)) {
        dwarf_dealloc_die(cu_die);
        printf_e("ocdwarf_die_and_siblings failed! %d", n1);
        return n1;
      }

      n0 += n1;

      dwarf_dealloc_die(cu_die);
      ocdwarf_sfreset(p, &sf);
    }
  }

  return OCDWARF_ERRCODE(x, n0);
}

