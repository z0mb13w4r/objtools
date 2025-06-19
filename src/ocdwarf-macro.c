#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-macro.h"

static const int MAXSIZE = 24;

static const Dwarf_Sig8 zerosignature;

int ocdwarf_debug_macro(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = CAST(popcode_t, p);

    int level = 0;

    Dwarf_Bool isinfo = TRUE; /* our data is not DWARF4 .debug_types. */
    Dwarf_Unsigned next_cu_header_offset = 0;
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Unsigned type_offset    = 0;
    Dwarf_Half     extension_size = 0;
    Dwarf_Half     header_cu_type = 0;
    Dwarf_Half     address_size  = 0;
    Dwarf_Sig8     type_signature = zerosignature;
    Dwarf_Off      abbrev_offset = 0;

    Dwarf_Die      cu_die = 0;

    x = dwarf_next_cu_header_e(oc->items[OPCODE_DWARF], isinfo, &cu_die, &cu_header_length, &cu_version_stamp,
                     &abbrev_offset, &address_size, &cu_offset_size, &extension_size, &type_signature, &type_offset,
                     &next_cu_header_offset, &header_cu_type, oc->items[OPCODE_DWARF_ERROR]);

      if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
        printf_text("CU header length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(cu_header_length, USE_FHEX | USE_EOL);
        printf_text("Version stamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(cu_version_stamp, USE_DEC | USE_EOL);
        printf_text("Abbrev offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(abbrev_offset, USE_DEC | USE_EOL);
        printf_text("Address size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(address_size, USE_DEC | USE_EOL);
        printf_text("Extension size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(extension_size, USE_DEC | USE_EOL);
        printf_text("Type offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(type_offset, USE_FHEX | USE_EOL);
        printf_text("CU offset size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(cu_offset_size, USE_DEC | USE_EOL);
        printf_text("CU next header offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(next_cu_header_offset, USE_FHEX | USE_EOL);
        printf_text("CU header type", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(header_cu_type, USE_FHEX | USE_EOL);
      }

    Dwarf_Unsigned version = 0;
    Dwarf_Unsigned number_of_ops = 0;
    Dwarf_Unsigned macro_unit_offset = 0;
    Dwarf_Unsigned ops_total_byte_len = 0;
    Dwarf_Macro_Context macro_context = 0;

    x = dwarf_get_macro_context(cu_die, &version, &macro_context, &macro_unit_offset,
                     &number_of_ops, &ops_total_byte_len, oc->items[OPCODE_DWARF_ERROR]);

      if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
        printf_text("Macro unit offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(macro_unit_offset, USE_DEC | USE_EOL);
      }

    unsigned int macro_flags = 0;
    Dwarf_Half macro_version = 0;
    Dwarf_Unsigned macro_len = 0;
    Dwarf_Unsigned macro_offset = 0;
    Dwarf_Unsigned macro_header_len = 0;
    Dwarf_Unsigned line_offset = 0;
    Dwarf_Half opcode_count = 0;
    Dwarf_Bool has_line_offset = FALSE;
    Dwarf_Bool has_operands_table = FALSE;
    Dwarf_Bool has_offset_size_64 = FALSE;

    x = dwarf_macro_context_head(macro_context, &macro_version, &macro_offset, &macro_len, &macro_header_len,
                      &macro_flags, &has_line_offset, &line_offset, &has_offset_size_64, &has_operands_table,
                      &opcode_count, oc->items[OPCODE_DWARF_ERROR]);

    printf_text("Nested import level", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(level, USE_DEC | USE_EOL);

    printf_text("Macro version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(macro_version, USE_DEC | USE_EOL);

    printf_text("Macro section offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(macro_offset, USE_FHEX32 | USE_EOL);

    printf_text("MacroInformationEntries count", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    printf_nice(number_of_ops, USE_DEC);
    printf_text("bytes length", USE_LT | USE_COMMA | USE_COLON);
    printf_nice(ops_total_byte_len, USE_DEC | USE_EOL);

      if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
        printf_text("Macro header length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(macro_header_len, USE_DEC | USE_EOL);
        printf_text("Macro flags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(macro_flags, USE_DEC | USE_EOL);
        printf_text("Has line offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(has_line_offset, USE_YESNO | USE_EOL);
        printf_text("Line offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(line_offset, USE_DEC | USE_EOL);
        printf_text("Has offset size 64", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(has_offset_size_64, USE_YESNO | USE_EOL);
        printf_text("Has operands table", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(has_operands_table, USE_YESNO | USE_EOL);
        printf_text("Opcode count", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        printf_nice(opcode_count, USE_DEC | USE_EOL);
      }
  }

  return OCDWARF_ERRCODE(x, n0);
}

