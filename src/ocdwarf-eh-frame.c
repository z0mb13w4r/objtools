#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-eh-frame.h"

static const int MAXSIZE = 24;

int ocdwarf_eh_frame_cies(handle_t p, Dwarf_Die die, Dwarf_Cie *cie_data, Dwarf_Signed cie_element_count, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    n += printf_text("CIE", USE_LT | USE_SB | USE_EOL);

    for (Dwarf_Signed i = 0; i < cie_element_count; ++i) {
      char*          augmenter = "";
      Dwarf_Small    version = 0;
      Dwarf_Unsigned code_alignment_factor = 0;
      Dwarf_Signed   data_alignment_factor = 0;
      Dwarf_Small   *cie_initial_instructions = 0;
      Dwarf_Unsigned cie_initial_instructions_length = 0;
      Dwarf_Unsigned bytes_in_cie = 0;
      Dwarf_Half     address_register_rule = 0;
      Dwarf_Half     offset_size = 0;

      Dwarf_Cie cie = cie_data[i];
      x = dwarf_get_cie_info_b(cie, &bytes_in_cie, &version, &augmenter, &code_alignment_factor, &data_alignment_factor,
                     &address_register_rule, &cie_initial_instructions, &cie_initial_instructions_length, &offset_size, e);
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_get_cie_info_b failed! - %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      n += ocdwarf_printf_DEC(p, i, USE_NONE);
      n += printf_text("version", USE_LT | USE_SPACE | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(version, USE_DEC);
      n += printf_eol();

      Dwarf_Off cie_off = 0;
      x = dwarf_cie_section_offset(ocget(p, OPCODE_DWARF_DEBUG), cie, &cie_off, e);
      if (IS_DLV_OK(x)) {
        n += printf_text("cie section offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(cie_off, USE_DEC);
        n += printf_nice(cie_off, USE_FHEX32);
        n += printf_eol();
      }

      n += printf_text("augmentation", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_text(augmenter, USE_LT);
      n += printf_eol();

      n += printf_text("code_alignment_factor", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(code_alignment_factor, USE_DEC);
      n += printf_eol();

      n += printf_text("data_alignment_factor", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(data_alignment_factor, USE_DEC);
      n += printf_eol();

      n += printf_text("address_register_rule", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(address_register_rule, USE_DEC);
      n += printf_eol();

      Dwarf_Small *augdata = 0;
      Dwarf_Unsigned augdata_len = 0;
      n += dwarf_get_cie_augmentation_data(cie, &augdata, &augdata_len, e);
      if (IS_DLV_OK(x)) {
        n += printf_text("eh aug data len", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(augdata_len, USE_FHEX);
        n += printf_text("bytes", USE_LT | USE_COMMA);
        n += printf_hurt(augdata, augdata_len, USE_HEX | USE_SPACE | USE_0x);
        n += printf_eol();
      }

      n += printf_text("bytes of initial instructions", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(cie_initial_instructions_length, USE_DEC);
      n += printf_eol();

      n += printf_text("cie length", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(bytes_in_cie, USE_DEC);
      n += printf_eol();

      Dwarf_Frame_Instr_Head instr_head = 0;
      Dwarf_Unsigned         instr_count = 0;
      x = dwarf_expand_frame_instructions(cie, cie_initial_instructions, cie_initial_instructions_length,
                     &instr_head, &instr_count, e);
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_expand_frame_instructions failed! - %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      if (0 != instr_count) {
        printf_text("[  ] offset name                 operands", USE_LT | USE_TAB | USE_EOL);
      }

      for (Dwarf_Unsigned j = 0; j < instr_count; ++j) {
        const char     *fields_description = 0;
        Dwarf_Small     cfa_operation = 0;
        Dwarf_Unsigned  instr_offset_in_instrs = 0;
        Dwarf_Unsigned  u0 = 0;
        Dwarf_Unsigned  u1 = 0;
        Dwarf_Unsigned  u2 = 0;
        Dwarf_Signed    s0 = 0;
        Dwarf_Signed    s1 = 0;
        Dwarf_Block     expression_block;

        x = dwarf_get_frame_instruction_a(instr_head, j, &instr_offset_in_instrs, &cfa_operation,
                     &fields_description, &u0, &u1, &u2, &s0, &s1,
                     0, 0, /* These alignment factors passed to us. */
                     &expression_block, e);
        if (IS_DLV_NO_ENTRY(x)) break;
        else if (IS_DLV_ERROR(x)) {
          printf_e("dwarf_get_frame_instruction_a failed! - %d", x);
          ocdwarf_dealloc_error(p, e);
          return OCDWARF_ERRCODE(x, n);
        }

        n += ocdwarf_printf_DEC(p, j, USE_SB);
        n += printf_nice(instr_offset_in_instrs, USE_DEC2);
        n += ocdwarf_printf_CFA(p, cfa_operation, USE_NONE);
        n += ocdwarf_printf_fields_description(p, die, fields_description, u0, u1, u2, s0, s1,
                     code_alignment_factor, data_alignment_factor, &expression_block);
        n += printf_eol();
      }

      dwarf_dealloc_frame_instr_head(instr_head);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_eh_frame_fdes(handle_t p, Dwarf_Fde *fde_data, Dwarf_Signed fde_element_count, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    n += printf_text("FDE", USE_LT | USE_SB | USE_EOL);

    for (Dwarf_Signed i = 0; i < fde_element_count; ++i) {
      Dwarf_Addr     low_pc = 0;
      Dwarf_Unsigned func_length = 0;
      Dwarf_Small   *fde_bytes = NULL;
      Dwarf_Unsigned fde_bytes_length = 0;
      Dwarf_Off      fde_offset = 0;
      Dwarf_Off      cie_offset = 0;
      Dwarf_Signed   cie_index = 0;

      Dwarf_Fde fde = fde_data[i];
      x = dwarf_get_fde_range(fde, &low_pc, &func_length, &fde_bytes, &fde_bytes_length,
                     &cie_offset, &cie_index, &fde_offset, e);
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_get_fde_range failed! - %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      Dwarf_Addr end_func_addr = low_pc + func_length;

      n += ocdwarf_printf_DEC(p, i, USE_NONE);

      // <0x00001020:0x000010c0>
      n += printf_nice(low_pc, USE_FHEX32 | USE_TBLT | USE_COLON | USE_NOSPACE);
      n += printf_nice(end_func_addr, USE_FHEX32 | USE_TBRT | USE_NOSPACE);

      // <_getchar>
      n += printf_text("_getchar", USE_LT | USE_TB);

      // <cie offset 0x00000034::cie index 0>
      n += printf_text("cie offset", USE_LT | USE_TBLT);
      n += printf_nice(cie_offset, USE_FHEX32);
      n += printf_text("::cie index", USE_LT);
      n += printf_nice(cie_index, USE_DEC | USE_TBRT);

      // <fde offset 0x00000030::length 0x00000024>
      n += printf_text("fde offset", USE_LT | USE_TBLT);
      n += printf_nice(fde_offset, USE_FHEX32);
      n += printf_text("::length", USE_LT);
      n += printf_nice(fde_bytes_length, USE_FHEX32 | USE_TBRT);
      n += printf_eol();

      Dwarf_Small *augdata = 0;
      Dwarf_Unsigned augdata_len = 0;
      x = dwarf_get_fde_augmentation_data(fde, &augdata, &augdata_len, e);
      if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_get_fde_augmentation_data failed! - %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      n += printf_text("eh aug data len", USE_LT | USE_TBLT);
      if (0 == augdata_len) {
        n += printf_nice(augdata_len, USE_FHEX | USE_TBRT);
      } else {
        n += printf_nice(augdata_len, USE_FHEX);
        n += printf_hurt(augdata, augdata_len, USE_HEX | USE_SPACE | USE_0x | USE_TBRT);
      }

      n += printf_eol();

      for (Dwarf_Addr j = low_pc; j < end_func_addr; ++j) {
        Dwarf_Addr     cur_pc_in_table = j;
        Dwarf_Addr     row_pc = 0;
        Dwarf_Addr     subsequent_pc = 0;
        Dwarf_Bool     has_more_rows = 0;
        Dwarf_Small    value_type = 0;
        Dwarf_Block    block = ZEROBLOCK;
        Dwarf_Signed   offset = 0;
        Dwarf_Unsigned offset_relevant = 0;
        Dwarf_Unsigned reg = 0;

        x = dwarf_get_fde_info_for_cfa_reg3_c(fde, j, &value_type, &offset_relevant,
                     &reg, &offset, &block, &row_pc, &has_more_rows, &subsequent_pc, e);
        if (IS_DLV_NO_ENTRY(x)) continue;
        else if (IS_DLV_ERROR(x)) {
          printf_e("dwarf_get_fde_info_for_cfa_reg3_c failed! - %d", x);
          return OCDWARF_ERRCODE(x, n);
        }

        n += printf_nice(j, USE_FHEX32 | USE_COLON);
        n += ocdwarf_printf_EXPR(p, value_type, USE_LT | USE_SPACE | USE_TBLT);
        n += printf_text("cfa=", USE_LT | USE_SPACE);
        if (DW_EXPR_EXPRESSION == value_type || DW_EXPR_VAL_EXPRESSION == value_type) {
          n += printf_text("expr-block-len=", USE_LT);
          n += printf_nice(block.bl_len, USE_DEC | USE_NOSPACE);
        } else {
          n += printf_nice(offset, USE_DEC2Z | USE_NOSPACE);
          n += printf_join("r", reg, USE_DEC | USE_RB);
        }
        n += printf_stop(USE_TBRT);

        if (!has_more_rows) {
          j = low_pc + func_length - 1;
        } else if (subsequent_pc > j) {
          j = subsequent_pc - 1;
        }

        for (Dwarf_Half k = 0; k < 100; ++k) {
          Dwarf_Addr row_pc = 0;

          x = dwarf_get_fde_info_for_reg3_c(fde, k, cur_pc_in_table, &value_type, &offset_relevant,
                     &reg, &offset, &block, &row_pc, &has_more_rows, &subsequent_pc, e);
          if (IS_DLV_NO_ENTRY(x)) continue;
          else if (IS_DLV_ERROR(x)) {
            printf_e("dwarf_get_fde_info_for_reg3_c failed! - %d", x);
            return OCDWARF_ERRCODE(x, n);
          } else if (row_pc != cur_pc_in_table) continue;

//          n += ocdwarf_printf_EXPR(p, value_type, USE_LT | USE_SPACE | USE_TBLT);
//          n += printf_nice(reg, USE_DEC);
        }

        n += printf_eol();
      }

      n += printf_eol();
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_eh_frame(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    Dwarf_Cie *cie_data = NULL;
    Dwarf_Fde *fde_data = NULL;
    Dwarf_Signed cie_element_count = 0;
    Dwarf_Signed fde_element_count = 0;
    x = dwarf_get_fde_list_eh(ocget(p, OPCODE_DWARF_DEBUG), &cie_data, &cie_element_count,
                     &fde_data, &fde_element_count, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_NO_ENTRY(x)) return n;
    else if (IS_DLV_ERROR(x)) {
      printf_e("dwarf_get_fde_list_eh failed! - %d", x);
      return OCDWARF_ERRCODE(x, n);
    }

    const char *sec_name = 0;
    x = dwarf_get_frame_section_name_eh_gnu(ocget(p, OPCODE_DWARF_DEBUG), &sec_name, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_ANY_ERROR(x) ||  0 == sec_name || 0 == *sec_name) {
      sec_name = ".eh_frame";
    }

    if (IS_DLV_ERROR(x)) {
      ocdwarf_dealloc_error(p, NULL);
    }

    Dwarf_Die die = 0;
    n += ocdwarf_eh_frame_fdes(p, fde_data, fde_element_count, ocget(p, OPCODE_DWARF_ERROR));
    n += ocdwarf_eh_frame_cies(p, die, cie_data, cie_element_count, ocget(p, OPCODE_DWARF_ERROR));

    ocdwarf_dealloc_fde_cie_list(p, cie_data, cie_element_count, fde_data, fde_element_count);
  }

  return OCDWARF_ERRCODE(x, n);
}

