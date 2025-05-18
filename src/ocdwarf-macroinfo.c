#include "printf.h"
#include "externs.h"
#include "ocdwarf-macroinfo.h"

int ocdwarf_debug_macroinfo0a(handle_t p, handle_t s, handle_t d, uint32_t offset1, uint32_t offset2, uint16_t level, uint16_t version, uint16_t count, uint16_t size) {
  const int MAXSIZE = 26;

  int n = 0;
  n += printf_text(".debug_macro: Macro info for a single cu at macro Offset", USE_LT);
  n += printf_nice(offset1, USE_FHEX32 | USE_EOL);

  n += printf_text("Macro data from CU-DIE at .debug_info offset", USE_LT);
  n += printf_nice(offset2, USE_FHEX32 | USE_COLON | USE_EOL);

  n += printf_text("Nested import level", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(level, USE_DEC | USE_EOL);

  n += printf_text("Macro version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(version, USE_DEC | USE_EOL);

  n += printf_text("Macro section offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
  n += printf_nice(offset1, USE_FHEX32 | USE_EOL);

  n += printf_text("MacroInformationEntries", USE_LT | USE_TAB | USE_COLON);
  n += printf_nice(count, USE_DEC | USE_COUNT);
  n += printf_nice(size, USE_DEC | USE_BYTES | USE_COMMA | USE_EOL);

  return n;
}

int ocdwarf_debug_macroinfo1a(handle_t p, handle_t s, handle_t d, uint16_t idx, uint8_t pick, uint32_t offset, uint16_t level, uint16_t version) {
  const int MAXSIZE1 = 23;
  const int MAXSIZE2 = 20;
  const int MAXSIZE3 = 10;

  int n = 0;
  n += printf_pack(2);
  n += printf_nice(idx, USE_DEC3 | USE_SB);
  n += printf_nice(pick, USE_FHEX8);
  n += printf_pick(zDWMACRO, pick, USE_LT | USE_SPACE | SET_PAD(MAXSIZE1));
  n += printf_text("offset", USE_LT);
  n += printf_nice(offset, USE_FHEX32 | USE_EOL);

  n += printf_pack(MAXSIZE3);
  n += printf_text("Nested import level", USE_LT | USE_COLON | SET_PAD(MAXSIZE2));
  n += printf_nice(level, USE_DEC | USE_EOL);

  n += printf_pack(MAXSIZE3);
  n += printf_text("Macro version", USE_LT | USE_COLON | SET_PAD(MAXSIZE2));
  n += printf_nice(version, USE_DEC | USE_EOL);

  n += printf_pack(MAXSIZE3);
  n += printf_text("macro section offset", USE_LT);
  n += printf_nice(offset, USE_FHEX32 | USE_EOL);

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

