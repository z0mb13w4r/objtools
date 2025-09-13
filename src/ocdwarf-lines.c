#include <stddef.h>
#include <dwarf.h>
#include <libdwarf.h>
#include <dwarf_base_types.h>
#include <dwarf_opaque.h>
#include <dwarf_line.h>

#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-lines.h"
#include "ocdwarf-magic.h"

static const int MAXSIZE = 24;

static int ocdwarf_debug_line0(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    pocdwarf_t ws = ocget(p, OPCODE_DWARF);
    popcode_t oc = ocget(p, OPCODE_THIS);

    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n += ocdwarf_printf_groups(p, ocget(p, OPCODE_DWARF_ERROR));
    }

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

    if (MODE_ISANY(oc->action, OPTPROGRAM_VERBOSE)) {
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

    Dwarf_Signed line_count = 0;
    Dwarf_Line  *line_array = NULL;
    Dwarf_Small  table_count = 0;
    Dwarf_Unsigned line_version = 0;
    Dwarf_Line_Context line_context = 0;

    x = dwarf_srclines_b(cu_die, &line_version, &table_count, &line_context, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_OK(x)) {
      if (MODE_ISANY(oc->action, OPTPROGRAM_VERBOSE)) {
        n += printf_text("Line version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(line_version, USE_DEC | USE_EOL);
        n += printf_text("Table count", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(table_count, USE_DEC | USE_EOL);
      }

      x = dwarf_srclines_from_linecontext(line_context, &line_array, &line_count, ocget(p, OPCODE_DWARF_ERROR));
    }

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

      n += printf_text("Source lines from CU-DIE at", USE_LT);
      n += printf_text(sec_name, USE_LT | USE_SPACE);
      n += printf_text("offset", USE_LT | USE_SPACE);
      n += printf_nice(offset, USE_FHEX32 | USE_COLON);
      n += printf_eol();
    } else {
      printf_text("Source lines (for the CU-DIE at unknown location)", USE_LT | USE_EOL);
      ocdwarf_dealloc_error(p, NULL);
    }

    n += ocdwarf_sfcreate(p, cu_die, ocget(p, OPCODE_DWARF_ERROR));

    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n += printf_text("NS new statement, BB new basic block, ET end of text sequence", USE_LT | USE_EOL);
      n += printf_text("PE prologue end, EB epilogue begin", USE_LT | USE_EOL);
      n += printf_text("IS=val ISA number, DI=val discriminator value", USE_LT | USE_EOL);
      n += printf_text("<pc> [lno,col] NS BB ET PE EB IS= DI= uri:", USE_LT);
      n += printf_text("filepath", USE_LT | USE_SPACE | USE_DQ | USE_EOL);
    }

    for (Dwarf_Signed i = 0; i < line_count; ++i) {
      if (MODE_ISANY(oc->action, OPTPROGRAM_VERBOSE)) {
        n += printf_nice(i, USE_DEC3 | USE_TB);
      }

      Dwarf_Addr pc = 0;
      Dwarf_Line k = line_array[i];
      x = dwarf_lineaddr(k, &pc, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_ERROR(x)) {
        pc = 0;
      } else if (IS_DLV_NO_ENTRY(x)) {
        pc = 0;
      }

      Dwarf_Unsigned nline = 0;
      x = dwarf_lineno(k, &nline, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_ERROR(x)) {
        nline = 0;
      } else if (IS_DLV_NO_ENTRY(x)) {
        nline = 0;
      }

      Dwarf_Unsigned column = 0;
      x = dwarf_lineoff_b(k, &column, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_ERROR(x)) {
        column = 0;
      } else if (IS_DLV_NO_ENTRY(x)) {
        column = 0;
      }

      if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        n += ocdwarf_printf_ADDR(p, pc, USE_NONE);
        n += printf_nice(nline, USE_DEC4 | USE_SBLT);
        n += printf_nice(column, USE_DEC2 | USE_COMMA | USE_SBRT);
      } else {
        char* lf = NULL;
        x = dwarf_linesrc(k, &lf, ocget(p, OPCODE_DWARF_ERROR));
        if (IS_DLV_OK(x) && 0 != lf && 0 != *lf) {
          n += printf_text(lf, USE_LT | USE_SHORTEN);
        }
        ocdwarf_dealloc(p, lf, DW_DLA_STRING);

        n += printf_nice(nline, USE_DEC4);
        n += ocdwarf_printf_ADDR(p, pc, USE_NONE);
      }

      Dwarf_Bool ns = FALSE;
      x = dwarf_linebeginstatement(k, &ns, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_OK(x) && ns) {
        n += printf_text(PICK_ENHANCED(oc, "NS", "x"), USE_LT | USE_SPACE);
      }

      if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        Dwarf_Bool bb = FALSE;
        x = dwarf_lineblock(k, &bb, ocget(p, OPCODE_DWARF_ERROR));
        if (IS_DLV_OK(x) && bb) {
          n += printf_text("BB", USE_LT | USE_SPACE);
        }
      }

      if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        Dwarf_Bool et = FALSE;
        x = dwarf_lineendsequence(k, &et, ocget(p, OPCODE_DWARF_ERROR));
        if (IS_DLV_OK(x) && et) {
          n += printf_text("ET", USE_LT | USE_SPACE);
        }
      }

      if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        Dwarf_Bool pe = FALSE;
        Dwarf_Bool eb = FALSE;
        Dwarf_Unsigned isa = 0;
        Dwarf_Unsigned discriminator = 0;
        x = dwarf_prologue_end_etc(k, &pe, &eb, &isa, &discriminator, ocget(p, OPCODE_DWARF_ERROR));
        if (IS_DLV_OK(x)) {
          if (pe) {
            n += printf_text("PE", USE_LT | USE_SPACE);
          }
          if (eb) {
            n += printf_text("EB", USE_LT | USE_SPACE);
          }
          if (isa) {
            n += printf_text("IS=", USE_LT | USE_SPACE);
            n += printf_nice(isa, USE_FHEX | USE_NOSPACE);
          }
          if (discriminator) {
            n += printf_text("DI=", USE_LT | USE_SPACE);
            n += printf_nice(discriminator, USE_FHEX | USE_NOSPACE);
          }
        }
      }

      if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        Dwarf_Unsigned cc = 0;
        x = dwarf_linecontext(k, &cc, ocget(p, OPCODE_DWARF_ERROR));
        if (IS_DLV_OK(x) && cc) {
          n += printf_text("CC=", USE_LT | USE_SPACE);
          n += printf_nice(cc, USE_FHEX | USE_NOSPACE);
        }
      }

      if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        char* sn = NULL;
        char* sf = NULL;
        Dwarf_Unsigned sl = 0;
        x = dwarf_line_subprog(k, &sn, &sf, &sl, ocget(p, OPCODE_DWARF_ERROR));
        if (IS_DLV_OK(x) && 0 != sn && 0 != *sn) {
          n += printf_text("SB=", USE_LT | USE_SPACE);
          n += printf_text(sn, USE_LT | USE_SPACE | USE_DQ);
        }
        ocdwarf_dealloc(p, sf, DW_DLA_STRING);
      }

      if ((0 == i) && MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        char* lf = NULL;
        x = dwarf_linesrc(k, &lf, ocget(p, OPCODE_DWARF_ERROR));
        if (IS_DLV_OK(x) && 0 != lf && 0 != *lf) {
          n += printf_text("uri", USE_LT | USE_COLON | USE_SPACE);
          n += printf_text(lf, USE_LT | USE_SPACE | USE_DQ);
        }
        ocdwarf_dealloc(p, lf, DW_DLA_STRING);
      }

      n += printf_eol();
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

static int ocdwarf_debug_line1(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
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

//    if (MODE_ISANY(oc->action, OPTPROGRAM_VERBOSE)) {
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
//    }

    Dwarf_Signed line_count = 0;
    Dwarf_Line  *line_array = NULL;
    Dwarf_Small  table_count = 0;
    Dwarf_Unsigned line_version = 0;

// struct Dwarf_Line_Context_s [dwarf_line.h:115]
    Dwarf_Line_Context line_context = 0;

    x = dwarf_srclines_b(cu_die, &line_version, &table_count, &line_context, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_OK(x)) {
//      if (MODE_ISANY(oc->action, OPTPROGRAM_VERBOSE)) {
        n += printf_text("Line version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(line_version, USE_DEC | USE_EOL);
        n += printf_text("Table count", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(table_count, USE_DEC | USE_EOL);
//      }

      x = dwarf_srclines_from_linecontext(line_context, &line_array, &line_count, ocget(p, OPCODE_DWARF_ERROR));
    }

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

      n += printf_text("Source lines from CU-DIE at", USE_LT);
      n += printf_text(sec_name, USE_LT | USE_SPACE);
      n += printf_text("offset", USE_LT | USE_SPACE);
      n += printf_nice(offset, USE_FHEX32 | USE_COLON);
      n += printf_eol();
    } else {
      printf_text("Source lines (for the CU-DIE at unknown location)", USE_LT | USE_EOL);
      ocdwarf_dealloc_error(p, NULL);
    }

    Dwarf_Bool is_stmt = FALSE;
    Dwarf_Unsigned total_length = 0;
    Dwarf_Unsigned section_offset = 0;
    Dwarf_Unsigned prologue_length = 0;
    Dwarf_Small minimum_instruction_length = 0;
    Dwarf_Ubyte maximum_ops_per_instruction = 0;
    Dwarf_Sbyte line_base = 0;
    Dwarf_Small line_range = 0;
    Dwarf_Small opcode_base = 0;

    x = dwarf_info_from_linecontext(line_context, &section_offset, &total_length, &prologue_length,
                                &minimum_instruction_length, &maximum_ops_per_instruction,
                                &is_stmt, &line_base, &line_range, &opcode_base, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_ANY_ERROR(x)) {
    }

printf("lc_unit_length %d[0x%x]\n", line_context->lc_unit_length, line_context->lc_unit_length);
printf("lc_length_field_length %d[0x%x]\n", line_context->lc_length_field_length, line_context->lc_length_field_length);
printf("lc_address_size %d[0x%x]\n", line_context->lc_address_size, line_context->lc_address_size);
printf("lc_segment_selector_size %d[0x%x]\n", line_context->lc_segment_selector_size, line_context->lc_segment_selector_size);
printf("lc_prologue_length %d[0x%x]\n", line_context->lc_prologue_length, line_context->lc_prologue_length);
printf("lc_actuals_table_offset %d[0x%x]\n", line_context->lc_actuals_table_offset, line_context->lc_actuals_table_offset);
printf("lc_logicals_table_offset %d[0x%x]\n", line_context->lc_logicals_table_offset, line_context->lc_logicals_table_offset);
printf("lc_minimum_instruction_length %d[0x%x]\n", line_context->lc_minimum_instruction_length, line_context->lc_minimum_instruction_length);
printf("lc_maximum_ops_per_instruction %d[0x%x]\n", line_context->lc_maximum_ops_per_instruction, line_context->lc_maximum_ops_per_instruction);
printf("lc_std_op_count %d[0x%x]\n", line_context->lc_std_op_count, line_context->lc_std_op_count);

    n += printf_text("Offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(section_offset, USE_FHEX | USE_EOL);
    n += printf_text("Length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(line_context->lc_total_length, USE_DEC | USE_EOL);
    n += printf_text("DWARF Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(line_version, USE_DEC | USE_EOL);
    n += printf_text("Prologue Length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(prologue_length, USE_DEC | USE_EOL);
    n += printf_text("Minimum Instruction Length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(minimum_instruction_length, USE_DEC | USE_EOL);
    n += printf_text("Initial value of 'is_stmt'", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(is_stmt, USE_DEC | USE_EOL);
    n += printf_text("Line Base", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(line_base, USE_DEC | USE_EOL);
    n += printf_text("Line Range", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(line_range, USE_DEC | USE_EOL);
    n += printf_text("Opcode Base", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
    n += printf_nice(opcode_base, USE_DEC | USE_EOL);

    n += ocdwarf_sfcreate(p, cu_die, ocget(p, OPCODE_DWARF_ERROR));

    n += printf_text("Opcodes", USE_LT | USE_COLON | USE_EOL);
    for (Dwarf_Small i = 1; i < opcode_base; ++i) {
      uint64_t args = 1;
      n += printf_text("Opcode", USE_LT);
      n += printf_nice(i, USE_DEC);
      n += printf_text("has", USE_LT | USE_SPACE);
      n += printf_nice(args, USE_DEC);
      n += printf_text(args ? "args" : "arg", USE_LT | USE_SPACE);
      n += printf_eol();
    }

    n += printf_text("The File Name Table", USE_LT);
    n += printf_nice(0, USE_FHEX | USE_OFFSET | USE_COLON | USE_EOL);
    n += printf_text("Entry Dir Time Size Name", USE_LT | USE_EOL);
    pdwarf_srcfiles_t sf = ocget(p, OPCODE_DWARF_SRCFILES);
    if (sf) {
      for (Dwarf_Signed i = 0; i < sf->size; ++i) {
        n += printf_nice(i + 1, USE_DEC3);
        n += printf_nice(0, USE_DEC);
        n += printf_nice(0, USE_DEC);
        n += printf_nice(0, USE_DEC);
        n += printf_text(sf->data[i], USE_LT | USE_SPACE | USE_SHORTEN | USE_EOL);
      }
    }

    n += printf_text("Line Number Statements", USE_LT | USE_COLON | USE_EOL);

    Dwarf_Addr prev_pc = 0;
    Dwarf_Bool prev_ns = line_context->lc_default_is_stmt;
    Dwarf_Unsigned prev_nline = 1;
    Dwarf_Unsigned prev_column = 0;

    uint64_t xx = prologue_length + 10;
    for (Dwarf_Signed i = 0; i < line_count; ++i) {
//      if (MODE_ISANY(oc->action, OPTPROGRAM_VERBOSE)) {
//        n += printf_nice(i, USE_DEC3 | USE_TB);
//      }

      Dwarf_Addr curr_pc = 0;
// struct Dwarf_Line_s [dwarf_line.h:303]
      Dwarf_Line k = line_array[i];
      x = dwarf_lineaddr(k, &curr_pc, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_ANY_ERROR(x)) {
        curr_pc = 0;
      }

      Dwarf_Unsigned curr_nline = 0;
      x = dwarf_lineno(k, &curr_nline, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_ANY_ERROR(x)) {
        curr_nline = 0;
      }

      Dwarf_Unsigned curr_column = 0;
      x = dwarf_lineoff_b(k, &curr_column, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_ANY_ERROR(x)) {
        curr_column = 0;
      }

      Dwarf_Bool isaddrset = FALSE;
      x = dwarf_line_is_addr_set(k, &isaddrset, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_OK(x)) {
      }

      Dwarf_Bool pe = FALSE;
      Dwarf_Bool eb = FALSE;
      Dwarf_Unsigned isa = 0;
      Dwarf_Unsigned discriminator = 0;
      x = dwarf_prologue_end_etc(k, &pe, &eb, &isa, &discriminator, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_OK(x)) {
      }

      Dwarf_Bool curr_ns = FALSE;
      x = dwarf_linebeginstatement(k, &curr_ns, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_OK(x)) {
      }

      Dwarf_Bool et = FALSE;
      x = dwarf_lineendsequence(k, &et, ocget(p, OPCODE_DWARF_ERROR));
      if (IS_DLV_OK(x)) {
      }

      if (curr_column != prev_column) {
        n += printf_nice(xx, USE_FHEX32 | USE_SB);
        n += printf_text("Set column to", USE_LT | USE_SPACE);
        n += printf_nice(curr_column, USE_DEC);
        n += printf_eol();
        xx += 2;
      }

      if (discriminator) {
        n += printf_nice(xx, USE_FHEX32 | USE_SB);
        n += printf_text("Extended opcode", USE_LT | USE_SPACE);
        n += printf_nice(DW_LNE_set_discriminator, USE_DEC | USE_COLON);
        n += printf_text("set Discriminator to", USE_LT | USE_SPACE);
        n += printf_nice(discriminator, USE_DEC);
        n += printf_eol();
        xx += 4;
      }

      if (curr_ns != prev_ns) {
        n += printf_nice(xx, USE_FHEX32 | USE_SB);
        n += printf_text("Set is_stmt to", USE_LT | USE_SPACE);
        n += printf_nice(curr_ns ? 1 : 0, USE_DEC);
        n += printf_eol();
        xx += 1;
      }

      if (isaddrset) {
        n += printf_nice(xx, USE_FHEX32 | USE_SB);
        n += printf_text("Extended opcode", USE_LT | USE_SPACE);
        n += printf_nice(DW_LNE_set_address, USE_DEC | USE_COLON);
        n += printf_text("set Address to", USE_LT | USE_SPACE);
        n += ocdwarf_printf_ADDR(p, curr_pc, USE_NONE);
        n += printf_eol();
        xx += ocis64(p) ? 11 : 7;

        n += printf_nice(xx, USE_FHEX32 | USE_SB);
        n += printf_text("Advance Line by", USE_LT | USE_SPACE);
        n += printf_nice(curr_nline - prev_nline, USE_DEC);
        n += printf_text("to", USE_LT | USE_SPACE);
        n += printf_nice(curr_nline, USE_DEC);
        n += printf_eol();
        xx += 2;

        n += printf_nice(xx, USE_FHEX32 | USE_SB);
        n += printf_text("Copy", USE_LT | USE_SPACE);
        n += printf_eol();
        xx += 1;
      } else if (et) {
        n += printf_nice(xx, USE_FHEX32 | USE_SB);
        n += printf_text("Advance PC by", USE_LT | USE_SPACE);
        n += printf_nice(curr_pc - prev_pc, USE_DEC);
        n += printf_text("to", USE_LT | USE_SPACE);
        n += ocdwarf_printf_ADDR(p, curr_pc, USE_NONE);
        n += printf_eol();
        xx += 2;

        n += printf_nice(xx, USE_FHEX32 | USE_SB);
        n += printf_text("Extended opcode", USE_LT | USE_SPACE);
        n += printf_nice(DW_LNE_end_sequence, USE_DEC | USE_COLON);
        n += printf_text("End of Sequence", USE_LT | USE_SPACE);
        n += printf_eol();
      } else {
        Dwarf_Addr size_pc = 17;
        Dwarf_Addr diff_pc = curr_pc - prev_pc;
        if (diff_pc >= (size_pc + size_pc)) {
          prev_pc += diff_pc;
          n += printf_nice(xx, USE_FHEX32 | USE_SB);
          n += printf_text("Advance PC by", USE_LT | USE_SPACE);
          n += printf_nice(diff_pc, USE_DEC);
          n += printf_text("to", USE_LT | USE_SPACE);
          n += ocdwarf_printf_ADDR(p, prev_pc, USE_NONE);
          n += printf_eol();
          xx += 2;
        } else if (diff_pc >= size_pc) {
          prev_pc += size_pc;
          n += printf_nice(xx, USE_FHEX32 | USE_SB);
          n += printf_text("Advance PC by constant", USE_LT | USE_SPACE);
          n += printf_nice(size_pc, USE_DEC);
          n += printf_text("to", USE_LT | USE_SPACE);
          n += ocdwarf_printf_ADDR(p, prev_pc, USE_NONE);
          n += printf_eol();
          xx += 1;
        }

        n += printf_nice(xx, USE_FHEX32 | USE_SB);
        n += printf_text("Special opcode", USE_LT | USE_SPACE);
        n += printf_nice(0, USE_DEC | USE_COLON);
        n += printf_text("advance Address by", USE_LT | USE_SPACE);
        n += printf_nice(curr_pc - prev_pc, USE_DEC);
        n += printf_text("to", USE_LT | USE_SPACE);
        n += ocdwarf_printf_ADDR(p, curr_pc, USE_NONE);
        n += printf_text("and Line by", USE_LT | USE_SPACE);
        n += printf_nice(curr_nline - prev_nline, USE_DEC);
        n += printf_text("to", USE_LT | USE_SPACE);
        n += printf_nice(curr_nline, USE_DEC);
        n += printf_eol();
        xx += 1;
      }

      prev_pc = curr_pc;
      prev_ns = curr_ns;
      prev_nline = curr_nline;
      prev_column = curr_column;
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_debug_line(handle_t p, handle_t s, handle_t d) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED | OPTDEBUGELF_DEBUG_LINE_DECODED)) {
      return ocdwarf_debug_line0(p, s, d);
    }
  }

  return ocdwarf_debug_line1(p, s, d);
}

