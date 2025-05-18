#include "printf.h"
#include "ocdwarf-macroinfo.h"

int ocdwarf_debug_macroinfo0a(handle_t p, handle_t s, handle_t d) {
  int n = 0;
  n += printf_text(".debug_macro: Macro info for a single cu at macro Offset 0x00000000", USE_LT | USE_EOL);
  n += printf_text("Macro data from CU-DIE at .debug_info offset 0x0000000b:", USE_LT | USE_EOL);
  n += printf_text("Nested import level: 0", USE_LT | USE_TAB | USE_EOL);
  n += printf_text("Macro version      : 4", USE_LT | USE_TAB | USE_EOL);
  n += printf_text("macro section offset 0x00000000", USE_LT | USE_TAB | USE_EOL);
  n += printf_text("MacroInformationEntries count: 232, bytes length: 803", USE_LT | USE_TAB | USE_EOL);

  return n;
}

int ocdwarf_debug_macroinfo1a(handle_t p, handle_t s, handle_t d) {
  int n = 0;
  n += printf_text(" [  0] 0x07 DW_MACRO_import       offset 0x0000032a", USE_LT | USE_TAB | USE_EOL);
  n += printf_text("        Nested import level: 1", USE_LT | USE_TAB | USE_EOL);
  n += printf_text("        Macro version      : 4", USE_LT | USE_TAB | USE_EOL);
  n += printf_text("        macro section offset 0x0000032a", USE_LT | USE_TAB | USE_EOL);

  return n;
}

