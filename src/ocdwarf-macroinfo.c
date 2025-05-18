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

int ocdwarf_debug_macroinfo2a(handle_t p, handle_t s, handle_t d) {
  int n = 0;
  n += printf_text(" [  1] 0x03 DW_MACRO_start_file   line 0 file number 1 /media/SHYLA/objtools/misc/debugLINUX64/../../src/example.c", USE_LT | USE_TAB | USE_EOL);

  return n;
}

int ocdwarf_debug_macroinfo3a(handle_t p, handle_t s, handle_t d) {
  int n = 0;
  n += printf_text(" [  4] 0x04 DW_MACRO_end_file", USE_LT | USE_TAB | USE_EOL);

  return n;
}

int ocdwarf_debug_macroinfo4a(handle_t p, handle_t s, handle_t d) {
  int n = 0;
  n += printf_text(" [  6] 0x05 DW_MACRO_define_strp  line 23 str offset 0x0000230a _CTYPE_H 1", USE_LT | USE_TAB | USE_EOL);

  return n;
}

