#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-eh-frame.h"

static const int MAXSIZE = 24;

typedef struct fdes_item_s {
  Dwarf_Signed   idx;
  Dwarf_Fde      fde;
  Dwarf_Addr     low_pc;
  Dwarf_Addr     high_pc;
  Dwarf_Unsigned func_length;
  Dwarf_Small   *fde_bytes;
  Dwarf_Unsigned fde_bytes_length;
  Dwarf_Off      cie_offset;
  Dwarf_Signed   cie_index;
  Dwarf_Off      fde_offset;
} fdes_item_t, *pfdes_item_t;

static int ocdwarf_eh_frame_cies(handle_t p, Dwarf_Cie *cie_data, Dwarf_Signed cie_element_count, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n += printf_text("CIE", USE_LT | USE_SB | USE_EOL);
    }

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

      if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        n += ocdwarf_printf_DEC(p, i, USE_NONE);
        n += printf_text("Version", USE_LT | USE_SPACE | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(version, USE_DEC);
        n += printf_eol();
      }

      Dwarf_Off cie_off = 0;
      x = dwarf_cie_section_offset(ocget(p, OPCODE_DWARF_DEBUG), cie, &cie_off, e);
      if (IS_DLV_OK(x)) {
        if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
          n += printf_text("CIE section offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
          n += printf_nice(cie_off, USE_DEC);
          n += printf_nice(cie_off, USE_FHEX32);
        } else {
          n += printf_nice(0x0, USE_LHEX32); // TBD saved_start - section_start
          n += printf_nice(bytes_in_cie, USE_LHEX32);
          n += printf_nice(cie_off, USE_LHEX32);
          n += printf_text("CIE", USE_LT | USE_SPACE);
        }
        n += printf_eol();
      }

      if (MODE_ISNOT(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        n += printf_text("Version", USE_LT | USE_SPACE | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(version, USE_DEC);
        n += printf_eol();
      }

      n += printf_text("Augmentation", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_text(augmenter, USE_LT | USE_DQ);
      n += printf_eol();

      n += printf_text("Code alignment factor", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(code_alignment_factor, USE_DEC);
      n += printf_eol();

      n += printf_text("Data alignment factor", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(data_alignment_factor, USE_DEC);
      n += printf_eol();

      n += printf_text("Return address column", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_nice(address_register_rule, USE_DEC);
      n += printf_eol();

      Dwarf_Small *augdata = 0;
      Dwarf_Unsigned augdata_len = 0;
      n += dwarf_get_cie_augmentation_data(cie, &augdata, &augdata_len, e);
      if (IS_DLV_OK(x)) {
        n += printf_text("Augmentation data", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
        if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
          n += printf_nice(augdata_len, USE_FHEX);
          n += printf_text("bytes", USE_LT | USE_COMMA);
        }
        n += printf_hurt(augdata, augdata_len, USE_HEX | USE_SPACE | PICK_ENHANCED(oc, USE_0x, USE_NONE));
        n += printf_eol();
      }

      if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
        n += printf_text("bytes of initial instructions", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(cie_initial_instructions_length, USE_DEC);
        n += printf_eol();

        n += printf_text("cie length", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        n += printf_nice(bytes_in_cie, USE_DEC);
        n += printf_eol();
      }

      Dwarf_Frame_Instr_Head instr_head = 0;
      Dwarf_Unsigned         instr_count = 0;
      x = dwarf_expand_frame_instructions(cie, cie_initial_instructions, cie_initial_instructions_length,
                     &instr_head, &instr_count, e);
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_expand_frame_instructions failed! - %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      if (0 != instr_count && MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
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

        const imode_t TRY_COLON = MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)
                     ? USE_NONE : (fields_description && fields_description[0] ? USE_COLON : USE_NONE);

        if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
          n += ocdwarf_printf_DEC(p, j, USE_SB);
          n += printf_nice(instr_offset_in_instrs, USE_DEC2);
        }
        n += ocdwarf_printf_CFA(p, cfa_operation, TRY_COLON);
        n += ocdwarf_printf_fields_description(p, fields_description, u0, u1, u2, s0, s1,
                     code_alignment_factor, data_alignment_factor, &expression_block);
        n += printf_eol();
      }

      dwarf_dealloc_frame_instr_head(instr_head);
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

static int ocdwarf_eh_frame_fdes0(handle_t p, Dwarf_Fde *fde_data, Dwarf_Signed fde_element_count, Dwarf_Error *e) {
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

      char* name = 0;
      n += ocdwarf_spget(p, low_pc, &name, NULL, NULL, NULL, NULL, NULL, NULL, NULL, e);

      // < 0>
      n += ocdwarf_printf_DEC(p, i, USE_NONE);

      // <0x00001020:0x000010c0>
      n += ocdwarf_printf_ADDR(p, low_pc, USE_TBLT | USE_COLON | USE_NOSPACE);
      n += ocdwarf_printf_ADDR(p, end_func_addr, USE_TBRT | USE_NOSPACE);

      // <_getchar>
      n += printf_text(name, USE_LT | USE_TB);

      // <cie offset 0x00000034::cie index 0>
      n += printf_text("cie offset", USE_LT | USE_TBLT);
      n += ocdwarf_printf_ADDR(p, cie_offset, USE_NONE);
      n += printf_text("::cie index", USE_LT);
      n += printf_nice(cie_index, USE_DEC | USE_TBRT);

      // <fde offset 0x00000030::length 0x00000024>
      n += printf_text("fde offset", USE_LT | USE_TBLT);
      n += ocdwarf_printf_ADDR(p, fde_offset, USE_NONE);
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
        Dwarf_Addr     cur_pc = j;
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

        n += ocdwarf_printf_ADDR(p, j, USE_COLON);
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
          Dwarf_Addr     row_pc = 0;
          Dwarf_Addr     subsequent_pc = 0;
          Dwarf_Bool     has_more_rows = 0;
          Dwarf_Small    value_type = 0;
          Dwarf_Block    block = ZEROBLOCK;
          Dwarf_Signed   offset = 0;
          Dwarf_Unsigned offset_relevant = 0;
          Dwarf_Unsigned reg = 0;

          x = dwarf_get_fde_info_for_reg3_c(fde, k, cur_pc, &value_type, &offset_relevant,
                     &reg, &offset, &block, &row_pc, &has_more_rows, &subsequent_pc, e);
          if (IS_DLV_ERROR(x)) {
            printf_e("dwarf_get_fde_info_for_reg3_c failed! - %d", x);
            return OCDWARF_ERRCODE(x, n);
          } else if (IS_DLV_NO_ENTRY(x) || row_pc != cur_pc || (0 == value_type && 0 == offset)) continue;

//printf("\n+++%d %d[k] %d[cpc] %d[vt] %d[ofr] %d[reg] %d[off] %d[rpc] %d[has] %d[spc]+++\n",
//  x, k, cur_pc, value_type, offset_relevant, reg, offset, row_pc, has_more_rows, subsequent_pc);

          n += ocdwarf_printf_EXPR(p, value_type, USE_LT | USE_TBLT);
          n += printf_join("r", k, USE_DEC | USE_SPACE);
          n += printf_text("=", USE_LT);
          if (DW_EXPR_EXPRESSION == value_type || DW_EXPR_VAL_EXPRESSION == value_type) {
            n += printf_text("expr-block-len=", USE_LT);
            n += printf_nice(block.bl_len, USE_DEC | USE_NOSPACE);
          } else {
            n += printf_nice(offset, USE_DEC | USE_NOSPACE);
            n += printf_text("cfa", USE_LT | USE_RB);
          }
          n += printf_stop(USE_TBRT);
        }

        n += printf_eol();
      }

      n += printf_eol();
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

static int ocdwarf_eh_frame_fdes1(handle_t p, Dwarf_Fde *fde_data, Dwarf_Signed fde_count, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    const imode_t USE_LHEXNN = ocis64(p) ? USE_LHEX64 : USE_LHEX32;

    pfdes_item_t fde_items = xmalloc(sizeof(pfdes_item_t) * fde_count);
    pfdes_item_t fde_item = fde_items;

    for (Dwarf_Signed i = 0; i < fde_count; ++i, ++fde_item) {
      fde_item->idx = i;
      fde_item->fde = fde_data[i];
      x = dwarf_get_fde_range(fde_item->fde, &fde_item->low_pc, &fde_item->func_length,
                     &fde_item->fde_bytes, &fde_item->fde_bytes_length, &fde_item->cie_offset,
                     &fde_item->cie_index, &fde_item->fde_offset, e);
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_get_fde_range failed! - %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      fde_item->high_pc = fde_item->low_pc + fde_item->func_length;
    }

    fde_item = fde_items;
    for (Dwarf_Signed i = 0; i < fde_count; ++i, ++fde_item) {
      char* name = 0;
      n += ocdwarf_spget(p, fde_item->low_pc, &name, NULL, NULL, NULL, NULL, NULL, NULL, NULL, e);

      n += printf_nice(fde_item->fde_offset, USE_LHEX32);
      n += printf_nice(fde_item->fde_bytes_length, USE_LHEXNN);
      n += printf_nice(fde_item->cie_offset, USE_LHEX32);
      n += printf_text("FDE cie=", USE_LT | USE_SPACE);
      n += printf_nice(fde_item->cie_index, USE_LHEX32 | USE_NOSPACE);
      n += printf_text("pc=", USE_LT | USE_SPACE);
      n += printf_nice(fde_item->low_pc, USE_LHEXNN | USE_NOSPACE);
      n += printf_text("..", USE_LT);
      n += printf_nice(fde_item->high_pc, USE_LHEXNN | USE_NOSPACE);
      n += printf_eol();

      Dwarf_Small *augdata = 0;
      Dwarf_Unsigned augdata_len = 0;
      x = dwarf_get_fde_augmentation_data(fde_item->fde, &augdata, &augdata_len, e);
      if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_get_fde_augmentation_data failed! - %d", x);
        return OCDWARF_ERRCODE(x, n);
      }

      for (Dwarf_Addr j = fde_item->low_pc; j < fde_item->high_pc; ++j) {
        Dwarf_Addr     cur_pc = j;
        Dwarf_Addr     row_pc = 0;
        Dwarf_Addr     subsequent_pc = 0;
        Dwarf_Bool     has_more_rows = 0;
        Dwarf_Small    value_type = 0;
        Dwarf_Block    block = ZEROBLOCK;
        Dwarf_Signed   offset = 0;
        Dwarf_Unsigned offset_relevant = 0;
        Dwarf_Unsigned reg = 0;

        x = dwarf_get_fde_info_for_cfa_reg3_c(fde_item->fde, j, &value_type, &offset_relevant,
                     &reg, &offset, &block, &row_pc, &has_more_rows, &subsequent_pc, e);
        if (IS_DLV_NO_ENTRY(x)) continue;
        else if (IS_DLV_ERROR(x)) {
          printf_e("dwarf_get_fde_info_for_cfa_reg3_c failed! - %d", x);
          return OCDWARF_ERRCODE(x, n);
        }

//        n += ocdwarf_printf_ADDR(p, j, USE_COLON);
//        n += ocdwarf_printf_EXPR(p, value_type, USE_LT | USE_SPACE | USE_TBLT);
//        n += printf_text("cfa=", USE_LT | USE_SPACE);
//        if (DW_EXPR_EXPRESSION == value_type || DW_EXPR_VAL_EXPRESSION == value_type) {
//          n += printf_text("expr-block-len=", USE_LT);
//          n += printf_nice(block.bl_len, USE_DEC | USE_NOSPACE);
//        } else {
//          n += printf_nice(offset, USE_DEC2Z | USE_NOSPACE);
//          n += printf_join("r", reg, USE_DEC | USE_RB);
//        }
//        n += printf_stop(USE_TBRT);

        if (!has_more_rows) {
          j = fde_item->high_pc - 1;
        } else if (subsequent_pc > j) {
          j = subsequent_pc - 1;
        }

//        for (Dwarf_Half k = 0; k < 100; ++k) {
//          Dwarf_Addr     row_pc = 0;
//          Dwarf_Addr     subsequent_pc = 0;
//          Dwarf_Bool     has_more_rows = 0;
//          Dwarf_Small    value_type = 0;
//          Dwarf_Block    block = ZEROBLOCK;
//          Dwarf_Signed   offset = 0;
//          Dwarf_Unsigned offset_relevant = 0;
//          Dwarf_Unsigned reg = 0;
//
//          x = dwarf_get_fde_info_for_reg3_c(fde, k, cur_pc, &value_type, &offset_relevant,
//                     &reg, &offset, &block, &row_pc, &has_more_rows, &subsequent_pc, e);
//          if (IS_DLV_ERROR(x)) {
//            printf_e("dwarf_get_fde_info_for_reg3_c failed! - %d", x);
//            return OCDWARF_ERRCODE(x, n);
//          } else if (IS_DLV_NO_ENTRY(x) || row_pc != cur_pc || (0 == value_type && 0 == offset)) continue;

//printf("\n+++%d %d[k] %d[cpc] %d[vt] %d[ofr] %d[reg] %d[off] %d[rpc] %d[has] %d[spc]+++\n",
//  x, k, cur_pc, value_type, offset_relevant, reg, offset, row_pc, has_more_rows, subsequent_pc);

//          n += ocdwarf_printf_EXPR(p, value_type, USE_LT | USE_TBLT);
//          n += printf_join("r", k, USE_DEC | USE_SPACE);
//          n += printf_text("=", USE_LT);
//          if (DW_EXPR_EXPRESSION == value_type || DW_EXPR_VAL_EXPRESSION == value_type) {
//            n += printf_text("expr-block-len=", USE_LT);
//            n += printf_nice(block.bl_len, USE_DEC | USE_NOSPACE);
//          } else {
//            n += printf_nice(offset, USE_DEC | USE_NOSPACE);
//            n += printf_text("cfa", USE_LT | USE_RB);
//          }
//          n += printf_stop(USE_TBRT);
//        }
//
//        n += printf_eol();
      }

      n += printf_eol();
    }

    xfree(fde_items);
  }

  return OCDWARF_ERRCODE(x, n);
}

int ocdwarf_eh_frame(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = ocget(p, OPCODE_THIS);

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

    if (MODE_ISANY(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n += ocdwarf_printf_groups(p, ocget(p, OPCODE_DWARF_ERROR));
      n += ocdwarf_eh_frame_fdes0(p, fde_data, fde_element_count, ocget(p, OPCODE_DWARF_ERROR));
      n += ocdwarf_eh_frame_cies(p, cie_data, cie_element_count, ocget(p, OPCODE_DWARF_ERROR));
    } else {
      n += ocdwarf_eh_frame_cies(p, cie_data, cie_element_count, ocget(p, OPCODE_DWARF_ERROR));
      n += ocdwarf_eh_frame_fdes1(p, fde_data, fde_element_count, ocget(p, OPCODE_DWARF_ERROR));
    }

    ocdwarf_dealloc_fde_cie_list(p, cie_data, cie_element_count, fde_data, fde_element_count);
  }

  return OCDWARF_ERRCODE(x, n);
}

