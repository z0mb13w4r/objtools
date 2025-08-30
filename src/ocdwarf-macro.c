#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-macro.h"

static const int MAXSIZE = 24;

#define PICK_ENHANCED(x,y,z)           (MODE_ISANY((x)->ocdump, OPTDEBUGELF_ENHANCED) ? (y) : (z))

int ocdwarf_debug_macro_ops(handle_t p, Dwarf_Die die, Dwarf_Macro_Context context, int level,
                     Dwarf_Half macro_version, Dwarf_Unsigned macro_unit_offset, Dwarf_Unsigned count, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    for (Dwarf_Unsigned i = 0; i < count; ++i) {
      Dwarf_Half formcodes_count = 0;
      Dwarf_Half macro_operator = 0;
      const Dwarf_Small *formcodes_array = NULL;
      Dwarf_Unsigned op_start_section_offset = 0;
      x = dwarf_get_macro_op(context, i, &op_start_section_offset, &macro_operator,
                       &formcodes_count, &formcodes_array, e);
      if (IS_DLV_ANY_ERROR(x)) {
        printf_e("dwarf_get_macro_op failed! - %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        n += ocdwarf_printf_DEC(p, i, USE_SB);
      }
      n += ocdwarf_printf_MACRO(p, macro_operator, USE_SPECIAL);

      const imode_t TRY_COLON = MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED) ? USE_NONE : USE_COLON;

      if (MODE_ISNOT(oc->ocdump, OPTDEBUGELF_ENHANCED) && (DW_MACRO_end_file != macro_operator) && (0 != macro_operator)) {
        n += printf_text("-", USE_LT | USE_SPACE);
      }

      if (DW_MACRO_import == macro_operator || DW_MACRO_import_sup == macro_operator) {
        Dwarf_Unsigned macro_offset = 0;

        x = dwarf_get_macro_import(context, i, &macro_offset, e);
        if (IS_DLV_ANY_ERROR(x)) {
          printf_e("dwarf_get_macro_import failed! - %d", x);
          return OCDWARF_ERRCODE(x, n);
        }

        if (DW_MACRO_import == macro_operator) {
          n += printf_text("offset", USE_LT | USE_SPACE | TRY_COLON);
          n += ocdwarf_printf_ADDR(p, macro_offset, USE_EOL);

          if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
            n += printf_text("Nested import level", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
            n += printf_nice(level + 1, USE_DEC | USE_EOL);
            n += printf_text("Macro version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
            n += printf_nice(macro_version, USE_DEC | USE_EOL);
            n += printf_text("Macro section offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
            n += printf_nice(macro_offset, USE_FHEX32 | USE_EOL);
          }
        } else if (DW_MACRO_import_sup == macro_operator) {
          n += printf_text("sup_offset", USE_LT | USE_SPACE);
          n += printf_nice(macro_offset, USE_FHEX32);
        }
      } else if (DW_MACRO_start_file == macro_operator) {
        pdwarf_statistics_t st = ocget(p, OPCODE_DWARF_STATISTICS);

        Dwarf_Unsigned nline = 0;
        Dwarf_Unsigned index = 0;
        const char    *name_string = NULL;

        x = dwarf_get_macro_startend_file(context, i, &nline, &index, &name_string, e);
        if (IS_DLV_ANY_ERROR(x)) {
          printf_e("dwarf_get_macro_startend_file failed! - %d", x);
          return OCDWARF_ERRCODE(x, n);
        }

        st->sdepth ++;

        n += printf_text(PICK_ENHANCED(oc, "line", "lineno"), USE_LT | USE_SPACE | TRY_COLON);
        n += printf_nice(nline, USE_DEC);
        n += printf_text(PICK_ENHANCED(oc, "file number", "filenum"), USE_LT | USE_SPACE | TRY_COLON);
        n += ocdwarf_printf_SRCFILE(p, index, USE_DEC);
      } else if (isused(zMACRODEF, macro_operator)) {
        Dwarf_Unsigned nline = 0;
        Dwarf_Unsigned index = 0;
        Dwarf_Unsigned offset = 0;
        Dwarf_Half     forms_count = 0;
        const char    *macro_string = NULL;

        x = dwarf_get_macro_defundef(context, i, &nline, &index, &offset, &forms_count, &macro_string, e);
        if (IS_DLV_ANY_ERROR(x)) {
          printf_e("dwarf_get_macro_defundef failed! - %d", x);
          return OCDWARF_ERRCODE(x, n);
        }

        n += printf_text(PICK_ENHANCED(oc, "line", "lineno"), USE_LT | USE_SPACE | TRY_COLON);
        n += printf_nice(nline, USE_DEC);

        if (isused(zMACRODEFSTR, macro_operator) && MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
          n += printf_text("str offset", USE_LT | USE_SPACE | TRY_COLON);
          n += printf_nice(offset, USE_FHEX32);
        }

        if (macro_string) {
          if (MODE_ISNOT(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
            n += printf_text("macro", USE_LT | USE_SPACE | USE_COLON);
          }
          n += printf_text(macro_string, USE_LT | USE_SPACE);
        }
      } else if (0 == macro_operator) {
        if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
          n += printf_text("op offset", USE_LT | USE_SPACE);
          n += ocdwarf_printf_ADDR(p, op_start_section_offset, USE_NONE);
          n += printf_text("macro unit length", USE_LT | USE_SPACE);
          n += printf_nice(op_start_section_offset + 1 - macro_unit_offset, USE_DEC);
          n += printf_text("next byte offset", USE_LT | USE_SPACE);
          n += ocdwarf_printf_ADDR(p, op_start_section_offset + 1, USE_NONE);
          n += printf_eol();
        }

        n += ocdwarf_debug_macro_offset(p, die, level, op_start_section_offset + 1, e);
      }

      n += printf_eol();
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_debug_macro_offset(handle_t p, Dwarf_Die die, int level,
                     Dwarf_Unsigned macro_offset, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    pdwarf_statistics_t st = ocget(p, OPCODE_DWARF_STATISTICS);

    Dwarf_Unsigned version = 0;
    Dwarf_Unsigned number_of_ops = 0;
    Dwarf_Unsigned ops_total_byte_len = 0;
    Dwarf_Macro_Context macro_context = 0;

    st->mcount ++;
    st->eoffset = MAX(st->eoffset, macro_offset);

    x = dwarf_get_macro_context_by_offset(die, macro_offset, &version, &macro_context,
                     &number_of_ops, &ops_total_byte_len, e);
    if (IS_DLV_OK(x)) {
      n += printf_text(".debug_macro: Macro info for imported macro unit at macro offset", USE_LT | USE_SPACE);
      n += ocdwarf_printf_ADDR(p, macro_offset, USE_COLON | USE_EOL);

      n += ocdwarf_debug_macro_context(p, die, macro_context, level, macro_offset,
                     number_of_ops, ops_total_byte_len, e);

      return OCDWARF_ERRCODE(x, n);
    }
  }

  return n;
}

int ocdwarf_debug_macro_context(handle_t p, Dwarf_Die die, Dwarf_Macro_Context context, int level,
                     Dwarf_Unsigned macro_unit_offset, Dwarf_Unsigned number_of_ops,
                     Dwarf_Unsigned ops_total_byte_len, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

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

    x = dwarf_macro_context_head(context, &macro_version, &macro_offset, &macro_len, &macro_header_len,
                      &macro_flags, &has_line_offset, &line_offset, &has_offset_size_64, &has_operands_table,
                      &opcode_count, e);
    if (IS_DLV_ANY_ERROR(x)) {
      printf_e("dwarf_macro_context_head failed! - %d", x);
      return OCDWARF_ERRCODE(x, n);
    }

    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n += printf_text("Nested import level", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(level, USE_DEC | USE_EOL);

      n += printf_text("Macro version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(macro_version, USE_DEC | USE_EOL);

      n += printf_text("Macro section offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(macro_offset, USE_FHEX32 | USE_EOL);

      n += printf_text("MacroInformationEntries count", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(number_of_ops, USE_DEC);
      n += printf_text("bytes length", USE_LT | USE_COMMA | USE_COLON);
      n += printf_nice(ops_total_byte_len, USE_DEC | USE_EOL);
    }

    if (MODE_ISANY(oc->action, OPTPROGRAM_VERBOSE)) {
      n += printf_text("Macro header length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(macro_header_len, USE_DEC | USE_EOL);
      n += printf_text("Macro flags", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(macro_flags, USE_DEC | USE_EOL);
      n += printf_text("Has line offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(has_line_offset, USE_YESNO | USE_EOL);
      n += printf_text("Line offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(line_offset, USE_DEC | USE_EOL);
      n += printf_text("Has offset size 64", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(has_offset_size_64, USE_YESNO | USE_EOL);
      n += printf_text("Has operands table", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(has_operands_table, USE_YESNO | USE_EOL);
      n += printf_text("Opcode count", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(opcode_count, USE_DEC | USE_EOL);

      for (Dwarf_Half i = 0; i < opcode_count; ++i) {
        Dwarf_Half opcode_num = 0;
        Dwarf_Half operand_count = 0;
        const Dwarf_Small *operand_array = 0;

        x = dwarf_macro_operands_table(context, i,
                  &opcode_num, &operand_count, &operand_array, e);
        if (IS_DLV_NO_ENTRY(x)) {
          printf_e("dwarf_macro_operands_table - NO_ENTRY for index %u of %u indexes.", i, opcode_count);
          break;
        } else if (IS_DLV_ERROR(x)) {
          printf_x("dwarf_macro_operands_table - NO_ENTRY for index %u of %u indexes.", i, opcode_count);
        } else if (0 == opcode_num) {
          n += printf_nice(i, USE_DEC3 | USE_SB);
          n += printf_text("end of macro operands.", USE_LT | USE_EOL);
          continue;
        }

        n += ocdwarf_printf_DEC(p, i, USE_SB);
        n += printf_text("op", USE_LT | USE_SPACE | USE_COLON);
        n += ocdwarf_printf_MACRO(p, opcode_num, USE_NONE);
        n += printf_eol();
        for (Dwarf_Half j = 0; j < operand_count; ++j) {
          n += printf_nice(i, USE_DEC3 | USE_SB);
          n += ocdwarf_printf_FORM(p, operand_array[j], USE_NONE);
          n += printf_eol();
        }
        n += printf_eol();
      }
    }

    n += ocdwarf_debug_macro_ops(p, die, context, level, macro_version, macro_unit_offset, number_of_ops, e);
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_debug_macro(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    pdwarf_statistics_t st = ocget(p, OPCODE_DWARF_STATISTICS);
    popcode_t oc = ocget(p, OPCODE_THIS);

    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n += ocdwarf_printf_groups(p, ocget(p, OPCODE_DWARF_ERROR));
    }

    int level = 0;
    Dwarf_Die cu_die = 0;

    x = ocdwarf_next_cu_header(p, &cu_die, ocget(p, OPCODE_DWARF_ERROR));
    if (ECODE_ISNOENTRY(x)) return n;
    else if (ECODE_ISFAILED(x)) {
      ocdwarf_dealloc_error(p, NULL);
      return x;
    }

    n += x;

    Dwarf_Unsigned version = 0;
    Dwarf_Unsigned number_of_ops = 0;
    Dwarf_Unsigned ops_total_byte_len = 0;
    Dwarf_Macro_Context macro_context = 0;

    x = dwarf_get_macro_context(cu_die, &version, &macro_context, &st->soffset,
                     &number_of_ops, &ops_total_byte_len, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_NO_ENTRY(x)) return n;
    else if (IS_DLV_ERROR(x)) {
      printf_e("dwarf_get_macro_context failed! - %d", x);
      return OCDWARF_ERRCODE(x, n);
    }

    st->idepth ++;

    n += printf_text(".debug_macro: Macro info for a single cu at macro offset", USE_LT | USE_SPACE);
    n += printf_nice(st->soffset, USE_FHEX32 | USE_EOL);

    Dwarf_Off offset = 0;
    x = dwarf_dieoffset(cu_die, &offset, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_OK(x)) {
      const char *sec_name = 0;

      x = dwarf_get_die_section_name_b(cu_die, &sec_name, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_ANY_ERROR(x) ||  0 == sec_name || 0 == *sec_name) {
        sec_name = ".debug_info";
      }

      if (IS_DLV_ERROR(x)) {
        ocdwarf_dealloc_error(p, NULL);
      }

      n += printf_text("Macro data from CU-DIE at", USE_LT);
      n += printf_text(sec_name, USE_LT | USE_SPACE);
      n += printf_text("offset", USE_LT | USE_SPACE);
      n += printf_nice(offset, USE_FHEX32 | USE_COLON);
      n += printf_eol();
    } else {
      printf_text("Macro data (for the CU-DIE at unknown location)", USE_LT | USE_EOL);
      ocdwarf_dealloc_error(p, NULL);
    }

    if (MODE_ISANY(oc->action, OPTPROGRAM_VERBOSE)) {
      n += printf_text("Macro unit offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(st->soffset, USE_DEC | USE_EOL);
    }

    n += ocdwarf_sfcreate(p, cu_die, ocget(p, OPCODE_DWARF_ERROR));
    n += ocdwarf_debug_macro_context(p, cu_die, macro_context, level, st->soffset,
                     number_of_ops, ops_total_byte_len, ocget(p, OPCODE_DWARF_ERROR));

    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n += printf_text("Macro unit count DWARF5 .debug_macro", USE_LT | USE_COLON);
      n += printf_nice(st->mcount, USE_DEC);
      n += printf_eol();

      n += printf_text("Macro offsets start at", USE_LT);
      n += printf_nice(st->soffset, USE_FHEX32);
      n += printf_text("and end at", USE_LT | USE_SPACE);
      n += printf_nice(st->eoffset, USE_FHEX32);
      n += printf_eol();

      n += printf_text("Maximum nest depth of DW_MACRO_start_file", USE_LT | USE_COLON);
      n += printf_nice(st->sdepth, USE_DEC);
      n += printf_eol();

      n += printf_text("Maximum nest depth of DW_MACRO_import", USE_LT | USE_COLON);
      n += printf_nice(st->idepth, USE_DEC);
      n += printf_eol();
    }

    dwarf_dealloc_macro_context(macro_context);
  }

  return OCDWARF_ERRCODE(x, n);
}

