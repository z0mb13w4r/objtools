#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-lines.h"

static const int MAXSIZE = 24;

#define PICK_ENHANCED(x,y,z)           (MODE_ISANY((x)->ocdump, OPTDEBUGELF_ENHANCED) ? (y) : (z))

int ocdwarf_debug_line(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
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

