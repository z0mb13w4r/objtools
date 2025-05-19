#include "printf.h"
#include "externs.h"
#include "ocdwarf-macroinfo.h"

static const int MAXSIZE = 23;

int ocdwarf_debug_macroinfo0a(handle_t p, handle_t s, handle_t d, const uint32_t offset1, const uint32_t offset2,
                              const uint16_t level, const uint16_t version, const uint16_t count, const uint16_t size) {
  const int MAXSIZE2 = 26;

  int n = 0;
  n += printf_text(".debug_macro: Macro info for a single cu at macro Offset", USE_LT);
  n += printf_nice(offset1, USE_FHEX32 | USE_EOL);

  n += printf_text("Macro data from CU-DIE at .debug_info offset", USE_LT);
  n += printf_nice(offset2, USE_FHEX32 | USE_COLON | USE_EOL);

  n += printf_text("Nested import level", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE2));
  n += printf_nice(level, USE_DEC | USE_EOL);

  n += printf_text("Macro version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE2));
  n += printf_nice(version, USE_DEC | USE_EOL);

  n += printf_text("Macro section offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE2));
  n += printf_nice(offset1, USE_FHEX32 | USE_EOL);

  n += printf_text("MacroInformationEntries", USE_LT | USE_TAB | USE_COLON);
  n += printf_nice(count, USE_DEC | USE_COUNT);
  n += printf_nice(size, USE_DEC | USE_BYTES | USE_COMMA | USE_EOL);

  return n;
}

int ocdwarf_debug_macroinfo1a(handle_t p, handle_t s, handle_t d, const uint16_t idx, const uint8_t pick, const uint32_t offset,
                              const uint16_t level, const uint16_t version) {
  const int MAXSIZE2 = 20;
  const int MAXSIZE3 = 10;

  int n = 0;
  n += printf_pack(2);
  n += printf_nice(idx, USE_DEC3 | USE_SB);
  n += printf_nice(pick, USE_FHEX8);
  n += printf_pick(zDWMACRO, pick, USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
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

int ocdwarf_debug_macroinfo2a(handle_t p, handle_t s, handle_t d, const uint16_t idx, const uint8_t pick,
                              const uint16_t lineno, const uint16_t fileno, const char* name) {
  int n = 0;
  n += printf_pack(2);
  n += printf_nice(idx, USE_DEC3 | USE_SB);
  n += printf_nice(pick, USE_FHEX8);
  n += printf_pick(zDWMACRO, pick, USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
  n += printf_text("line", USE_LT);
  n += printf_nice(lineno, USE_DEC);
  n += printf_text("file number", USE_LT | USE_SPACE);
  n += printf_nice(fileno, USE_DEC);
  n += printf_text(name, USE_LT | USE_SPACE | USE_EOL);

  return n;
}

int ocdwarf_debug_macroinfo3a(handle_t p, handle_t s, handle_t d, const uint16_t idx, const uint8_t pick) {
  int n = 0;
  n += printf_pack(2);
  n += printf_nice(idx, USE_DEC3 | USE_SB);
  n += printf_nice(pick, USE_FHEX8);
  n += printf_pick(zDWMACRO, pick, USE_LT | USE_SPACE | USE_EOL);

  return n;
}

int ocdwarf_debug_macroinfo4a(handle_t p, handle_t s, handle_t d, const uint16_t idx, const uint8_t pick,
                              const uint16_t lineno, const uint32_t offset, const char* name) {
  int n = 0;
  n += printf_pack(2);
  n += printf_nice(idx, USE_DEC3 | USE_SB);
  n += printf_nice(pick, USE_FHEX8);
  n += printf_pick(zDWMACRO, pick, USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
  n += printf_text("line", USE_LT);
  n += printf_nice(lineno, USE_DEC);
  n += printf_text("str offset", USE_LT | USE_SPACE);
  n += printf_nice(offset, USE_FHEX32);
  n += printf_text(name, USE_LT | USE_SPACE | USE_EOL);

  return n;
}

