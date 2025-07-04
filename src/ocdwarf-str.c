#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-str.h"

static const int MAXSIZE = 24;

int ocdwarf_debug_str(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    Dwarf_Unsigned group_count = 0;
    Dwarf_Unsigned section_count = 0;
    Dwarf_Unsigned selected_group = 0;
    Dwarf_Unsigned group_map_entry_count = 0;

    x = dwarf_sec_group_sizes(ocget(p, OPCODE_DWARF_DEBUG), &section_count, &group_count, &selected_group,
                     &group_map_entry_count, ocget(p, OPCODE_DWARF_ERROR));

    if (IS_DLV_OK(x) && (group_count > 0)) {
      n += printf_text("Section Groups data", USE_LT | USE_EOL);
      n += printf_text("Number of Elf-like sections", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(section_count, USE_DEC | USE_EOL);
      n += printf_text("Number of groups", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(group_count, USE_DEC | USE_EOL);
      n += printf_text("Group to print", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += ocdwarf_printf_GNUM(p, selected_group, USE_SPECIAL | USE_EOL);
      n += printf_text("Count of map entries", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(group_map_entry_count, USE_DEC | USE_EOL);

      MALLOCA(Dwarf_Unsigned, sec_nums, group_map_entry_count);
      MALLOCA(Dwarf_Unsigned, group_nums, group_map_entry_count);
      MALLOCA(const char *, sec_names, group_map_entry_count);

      x = dwarf_sec_group_map(ocget(p, OPCODE_DWARF_DEBUG), group_map_entry_count,
                     group_nums, sec_nums, sec_names, ocget(p, OPCODE_DWARF_ERROR));

      if (IS_DLV_OK(x)) {
        for (Dwarf_Unsigned i = 0; i < group_map_entry_count; ++i) {
          if (i == 0) {
            printf_text("[index]  group section", USE_LT | USE_TAB | USE_EOL);
          }

          n += printf_nice(i, USE_DEC5 | USE_SB);
          n += printf_nice(group_nums[i], USE_DEC4);
          n += printf_nice(sec_nums[i], USE_DEC4);
          n += printf_text(sec_names[i], USE_LT | USE_SPACE);
          n += printf_eol();
        }
      }
    }

    char* string = 0;
    Dwarf_Off offset = 0;
    Dwarf_Signed length = 0;

    x = dwarf_get_str(ocget(p, OPCODE_DWARF_DEBUG), offset, &string, &length, ocget(p, OPCODE_DWARF_ERROR));
    while (IS_DLV_OK(x)) {
      n += printf_text("name at offset", USE_LT);
      n += printf_nice(offset, USE_FHEX32);
      n += printf_text("length", USE_LT | USE_COMMA);
      n += printf_nice(length, USE_DEC);
      n += printf_text("is", USE_LT | USE_SPACE);
      n += printf_text(string, USE_LT | USE_SPACE | USE_SQ);
      n += printf_eol();

      offset += length + 1;
      x = dwarf_get_str(ocget(p, OPCODE_DWARF_DEBUG), offset, &string, &length, ocget(p, OPCODE_DWARF_ERROR));
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

