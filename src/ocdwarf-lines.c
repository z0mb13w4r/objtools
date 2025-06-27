#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-lines.h"

int ocdwarf_debug_line(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    pocdwarf_t ws = ocget(p, OPCODE_DWARF);
    popcode_t oc = ocget(p, OPCODE_THIS);

    Dwarf_Bool isinfo = TRUE; /* our data is not DWARF4 .debug_types. */
    Dwarf_Unsigned next_cu_header_offset = 0;
    Dwarf_Unsigned cu_header_length = 0;
    Dwarf_Unsigned type_offset    = 0;
    Dwarf_Half     extension_size = 0;
    Dwarf_Half     header_cu_type = 0;
    Dwarf_Half     address_size  = 0;
    Dwarf_Sig8     type_signature = ZEROSIGNATURE;
    Dwarf_Off      abbrev_offset = 0;

    Dwarf_Die      cu_die = 0;

    x = dwarf_next_cu_header_e(ocget(p, OPCODE_DWARF_DEBUG), isinfo, &cu_die, &cu_header_length, &ws->cu_version_stamp,
                     &abbrev_offset, &address_size, &ws->cu_offset_size, &extension_size, &type_signature, &type_offset,
                     &next_cu_header_offset, &header_cu_type, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_NO_ENTRY(x)) return n;
    else if (IS_DLV_ERROR(x)) {
      printf_e("dwarf_next_cu_header_e failed! - %d", x);
      return OCDWARF_ERRCODE(x, n);
    }

    if (MODE_ISSET(oc->action, OPTPROGRAM_VERBOSE)) {
      n += printf_text("CU header length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(cu_header_length, USE_FHEX | USE_EOL);
      n += printf_text("Version stamp", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(ws->cu_version_stamp, USE_DEC | USE_EOL);
      n += printf_text("Abbrev offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(abbrev_offset, USE_DEC | USE_EOL);
      n += printf_text("Address size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(address_size, USE_DEC | USE_EOL);
      n += printf_text("Extension size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(extension_size, USE_DEC | USE_EOL);
      n += printf_text("Type offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(type_offset, USE_FHEX | USE_EOL);
      n += printf_text("CU offset size", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(ws->cu_offset_size, USE_DEC | USE_EOL);
      n += printf_text("CU next header offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(next_cu_header_offset, USE_FHEX | USE_EOL);
      n += printf_text("CU header type", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(header_cu_type, USE_FHEX | USE_EOL);
    }


    printf_text("NS new statement, BB new basic block, ET end of text sequence", USE_LT | USE_EOL);
    printf_text("PE prologue end, EB epilogue begin", USE_LT | USE_EOL);
    printf_text("IS=val ISA number, DI=val discriminator value", USE_LT | USE_EOL);

    if (MODE_ISSET(oc->ocdump, OPTDEBUGELF_DEBUG_LINE)) {
    }

    if (MODE_ISSET(oc->ocdump, OPTDEBUGELF_DEBUG_LINE_DECODED)) {
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

