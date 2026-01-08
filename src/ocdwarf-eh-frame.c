#include "printf.h"
#include "externs.h"
#include "options.h"
#include "opcode-engine.h"
#include "ocdwarf-eh-frame.h"

typedef struct fdes_item_s {
  Dwarf_Signed   idx;
  Dwarf_Fde      fde;
  Dwarf_Addr     lo_pc;
  Dwarf_Addr     hi_pc;
  Dwarf_Unsigned func_length;
  Dwarf_Small   *fde_bytes;
  Dwarf_Unsigned fde_bytes_length;
  Dwarf_Off      cie_offset;
  Dwarf_Signed   cie_index;
  Dwarf_Off      fde_offset;
} fdes_item_t, *pfdes_item_t;

#define REG_EAX                        (0)
#define REG_ECX                        (1)
#define REG_EDX                        (2)
#define REG_EBX                        (3)
#define REG_ESP                        (4)
#define REG_EBP                        (5)
#define REG_ESI                        (6)
#define REG_EDI                        (7)
#define REG_EIP                        (8)

static pick_t REGUSE32[] = {
  REG_EAX,
  REG_ECX,
  REG_EDX,
  REG_EBX,
  REG_EBP,
  REG_ESI,
  REG_EDI,
  PICK_END
};

#define REG_RAX                        (0)
#define REG_RDX                        (1)
#define REG_RCX                        (2)
#define REG_RBX                        (3)
#define REG_RSI                        (4)
#define REG_RDI                        (5)
#define REG_RBP                        (6)
#define REG_RSP                        (7)
#define REG_R8                         (8)
#define REG_R9                         (9)
#define REG_R10                        (10)
#define REG_R11                        (11)
#define REG_R12                        (12)
#define REG_R13                        (13)
#define REG_R14                        (14)
#define REG_R15                        (15)
#define REG_RIP                        (16)

static pick_t REGUSE64[] = {
  REG_RBX,
  REG_RBP,
  REG_R12,
  REG_R13,
  REG_R14,
  REG_R15,
  PICK_END
};

static pick_t REGUSEARM32[] = {
  PICK_END
};

#define REG_X19                        (19)
#define REG_X20                        (20)
#define REG_X21                        (21)
#define REG_X22                        (22)
#define REG_X23                        (23)
#define REG_X24                        (24)
#define REG_X29                        (29)
#define REG_X30                        (30)

static pick_t REGUSEARM64[] = {
  REG_X19,
  REG_X20,
  REG_X21,
  REG_X22,
  REG_X23,
  REG_X24,
  REG_X29,
  REG_X30,
  PICK_END
};

static pick_t REGUSERISCV32[] = {
  PICK_END
};

static ppick_t get_REGUSE(handle_t p) {
  if (EM_ARM == ocget_machine(p)) {
    return REGUSEARM32;
  } else if (EM_AARCH64 == ocget_machine(p)) {
    return REGUSEARM64;
  } else if (ocisELF32(p)) {
    if (EM_RISCV == ocget_machine(p)) {
      return REGUSERISCV32;
    }

    return REGUSE32;
  } else if (ocisELF64(p)) {
    return REGUSE64;
  }

  return NULL;
}

static int ocdwarf_eh_frame_cies(handle_t p, Dwarf_Cie *cie_data, Dwarf_Signed cie_element_count, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    const int MAXSIZE = 24;
    const int MAXSIZEN = 10;
    const int MAXSIZENN = ocis64(p) ? 16 : 8;

    const imode_t USE_LHEXNN = ocis64(p) ? USE_LHEX64 : USE_LHEX32;

    if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      n0 += printf_text("CIE", USE_LT | USE_SB | USE_EOL);
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
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("dwarf_get_cie_info_b failed! - %d", x);
#endif
        return OCDWARF_ERRCODE(x, n0);
      }

      if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
        n0 += ocdwarf_printf_DEC(p, i, USE_NONE);
        n0 += printf_text("Version", USE_LT | USE_SPACE | USE_COLON | SET_PAD(MAXSIZE));
        n0 += printf_nice(version, USE_DEC);
        n0 += printf_eol();
      }

      Dwarf_Off cie_off = 0;
      x = dwarf_cie_section_offset(ocget(p, OPCODE_DWARF_DEBUG), cie, &cie_off, e);
      if (IS_DLV_OK(x)) {
        if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
          n0 += printf_text("CIE section offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
          n0 += printf_nice(cie_off, USE_DEC);
          n0 += printf_nice(cie_off, USE_FHEX32);
          n0 += printf_eol();
        } else {
          n0 += printf_nice(0x0, USE_LHEX32 | USE_NOSPACE); // TBD saved_start - section_start
          n0 += printf_nice(bytes_in_cie, USE_LHEXNN);
          n0 += printf_nice(cie_off, USE_LHEX32);
          n0 += printf_text("CIE", USE_LT | USE_SPACE);
        }
      }

      if (MODE_ISNOT(oc->ocdump, OPTDWARF_ENHANCED | OPTDWARF_DEBUG_FRAME_DECODED)) {
        n0 += printf_eol();
        n0 += printf_text("Version", USE_LT | USE_SPACE | USE_COLON | SET_PAD(MAXSIZE));
        n0 += printf_nice(version, USE_DEC);
        n0 += printf_eol();
      }

      if (MODE_ISFIX(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED, OPTDWARF_ENHANCED)) {
        n0 += printf_text(augmenter, USE_LT | USE_SPACE | USE_DQ);
        n0 += printf_text("cf=", USE_LT | USE_SPACE);
        n0 += printf_nice(code_alignment_factor, USE_DEC | USE_NOSPACE);
        n0 += printf_text("df=", USE_LT | USE_SPACE);
        n0 += printf_nice(data_alignment_factor, USE_DEC | USE_NOSPACE);
        n0 += printf_text("ra=", USE_LT | USE_SPACE);
        n0 += printf_nice(address_register_rule, USE_DEC | USE_NOSPACE);
        n0 += printf_eol();
      } else {
        n0 += printf_text("Augmentation", USE_LT | USE_SPACE | USE_COLON | SET_PAD(MAXSIZE + 1));
        n0 += printf_text(augmenter, USE_LT | USE_DQ);
        n0 += printf_eol();

        n0 += printf_text("Code alignment factor", USE_LT | USE_SPACE | USE_COLON | SET_PAD(MAXSIZE));
        n0 += printf_nice(code_alignment_factor, USE_DEC);
        n0 += printf_eol();

        n0 += printf_text("Data alignment factor", USE_LT | USE_SPACE | USE_COLON | SET_PAD(MAXSIZE));
        n0 += printf_nice(data_alignment_factor, USE_DEC);
        n0 += printf_eol();

        n0 += printf_text("Return address column", USE_LT | USE_SPACE | USE_COLON | SET_PAD(MAXSIZE));
        n0 += printf_nice(address_register_rule, USE_DEC);
        n0 += printf_eol();
      }

      Dwarf_Small *augdata = 0;
      Dwarf_Unsigned augdata_len = 0;
      n0 += dwarf_get_cie_augmentation_data(cie, &augdata, &augdata_len, e);
      if (IS_DLV_OK(x)) {
        if (MODE_ISNOT(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED) || MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
          n0 += printf_text("Augmentation data", USE_LT | USE_SPACE | USE_COLON | SET_PAD(MAXSIZE + 1));
          if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
            n0 += printf_nice(augdata_len, USE_FHEX);
            n0 += printf_text("bytes", USE_LT | USE_COMMA);
          }
          n0 += printf_hurt(augdata, augdata_len, USE_HEX | PICK_ENHANCED(oc, USE_0x, USE_NONE));
          n0 += printf_eol();
        }
      }

      if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
        n0 += printf_text("Bytes of initial instructions", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        n0 += printf_nice(cie_initial_instructions_length, USE_DEC);
        n0 += printf_eol();

        n0 += printf_text("CIE length", USE_LT | USE_COLON | SET_PAD(MAXSIZE));
        n0 += printf_nice(bytes_in_cie, USE_DEC);
        n0 += printf_eol();
      }

      Dwarf_Frame_Instr_Head instr_head = 0;
      Dwarf_Unsigned         instr_count = 0;
      x = dwarf_expand_frame_instructions(cie, cie_initial_instructions, cie_initial_instructions_length,
                     &instr_head, &instr_count, e);
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("dwarf_expand_frame_instructions failed! - %d", x);
#endif
        return OCDWARF_ERRCODE(x, n0);
      }

      if (0 != instr_count && MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
        n0 += printf_text("[  ] off name                      operands", USE_LT | USE_SPACE | USE_EOL);
      } else if (MODE_ISANY(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED)) {
        n0 += printf_text("LOC", USE_LT | USE_TAB | SET_PAD(MAXSIZENN));
        n0 += printf_text("CFA", USE_LT | USE_SPACE | SET_PAD(MAXSIZEN));
        n0 += printf_text("ra", USE_LT | USE_SPACE);
        n0 += printf_eol();
      }

      if (MODE_ISFIX(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED, OPTDWARF_ENHANCED)) {
        n0 += printf_nice(cie_off, USE_LHEXNN | USE_NOSPACE);
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
#ifdef OPCODE_DWARF_DEBUGX
          printf_e("dwarf_get_frame_instruction_a failed! - %d", x);
#endif
          ocdwarf_dealloc_error(p, e);
          return OCDWARF_ERRCODE(x, n0);
        }

        const imode_t TRY_COLON = MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED | OPTDWARF_DEBUG_FRAME_DECODED)
                     ? USE_NONE : (fields_description && fields_description[0] ? USE_COLON : USE_NONE);

        if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
          n0 += ocdwarf_printf_DEC(p, j, USE_SB);
          n0 += printf_nice(instr_offset_in_instrs, USE_DEC3);
        }

        if (MODE_ISFIX(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED, OPTDWARF_ENHANCED)) {
          int n1 = 0;
          if (0 == j) {
            n1 += ocdwarf_printf_fields_description(p, cfa_operation, fields_description, u0, u1, u2, s0, s1,
                     code_alignment_factor, data_alignment_factor, &expression_block);
            n0 += printf_pack(MAXSIZEN - n1) + n1;
          } else if (DW_CFA_nop != cfa_operation) {
            n1 += ocdwarf_printf_CFA(p, cfa_operation, USE_NONE);
            n1 += printf_nice(u1 * data_alignment_factor, USE_DEC | USE_NOSPACE);
            n0 += printf_pack(MAXSIZEN - n1) + n1;
          }
        } else if (MODE_ISNOT(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED)) {
          n0 += ocdwarf_printf_CFA(p, cfa_operation, TRY_COLON);
          n0 += ocdwarf_printf_fields_description(p, cfa_operation, fields_description, u0, u1, u2, s0, s1,
                   code_alignment_factor, data_alignment_factor, &expression_block);

          n0 += printf_eol();
        } else {
          n0 += ocdwarf_printf_CFA(p, cfa_operation, TRY_COLON | SET_PAD(26));
          n0 += ocdwarf_printf_fields_description(p, cfa_operation, fields_description, u0, u1, u2, s0, s1,
                   code_alignment_factor, data_alignment_factor, &expression_block);

          n0 += printf_eol();
        }
      }

      if (MODE_ISFIX(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED, OPTDWARF_ENHANCED)) {
        n0 += printf_eol();
      }

      dwarf_dealloc_frame_instr_head(instr_head);
    }

    n0 += printf_eol();
  }

  return OCDWARF_ERRCODE(x, n0);
}

static int fdes_comp(const void* p0, const void* p1) {
  if (p0 && p1) {
    pfdes_item_t x0 = CAST(pfdes_item_t, p0);
    pfdes_item_t x1 = CAST(pfdes_item_t, p1);

    if (x0->fde_offset > x1->fde_offset) return  1;
    if (x0->fde_offset < x1->fde_offset) return -1;
  }

  return 0;
}

static int ocdwarf_eh_frame_fdes(handle_t p, Dwarf_Fde *fde_data, Dwarf_Signed fde_count, Dwarf_Error *e) {
  int x = DW_DLV_ERROR;
  int n0 = 0;

  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    const int MAXSIZE = 10;
    const int MAXSIZENN = ocis64(p) ? 16 : 8;

    const imode_t USE_LHEXNN = ocis64(p) ? USE_LHEX64 : USE_LHEX32;

    pfdes_item_t fde_items = xmalloc(sizeof(fdes_item_t) * fde_count);
    pfdes_item_t fde_item = fde_items;

    if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      n0 += printf_text("FDE", USE_LT | USE_SB | USE_EOL);
    }

    for (Dwarf_Signed i = 0; i < fde_count; ++i, ++fde_item) {
      fde_item->idx = i;
      fde_item->fde = fde_data[i];
      x = dwarf_get_fde_range(fde_item->fde, &fde_item->lo_pc, &fde_item->func_length,
                     &fde_item->fde_bytes, &fde_item->fde_bytes_length, &fde_item->cie_offset,
                     &fde_item->cie_index, &fde_item->fde_offset, e);
      if (IS_DLV_NO_ENTRY(x)) break;
      else if (IS_DLV_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("dwarf_get_fde_range failed! - %d", x);
#endif
        return OCDWARF_ERRCODE(x, n0);
      }

      fde_item->hi_pc = fde_item->lo_pc + fde_item->func_length;
    }

    if (MODE_ISNOT(oc->ocdump, OPTDWARF_ENHANCED)) {
      qsort (fde_items, fde_count, sizeof(fdes_item_t), fdes_comp);
    }

    fde_item = fde_items;
    for (Dwarf_Signed i = 0; i < fde_count; ++i, ++fde_item) {
      if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
        char* name = NULL;
        pocdebug_t d0 = oeseebyaddr(p, fde_item->lo_pc, OPENGINE_DEBUG);
        if (isodebug(d0) && MODE_ISANY(d0->role, OPDEBUG_NAME)) {
          name = d0->name;
        }

        // < 0>
        n0 += ocdwarf_printf_DEC(p, i, USE_NOSPACE);

        // <0x00001020:0x000010c0>
        n0 += ocdwarf_printf_ADDR(p, fde_item->lo_pc, USE_TBLT | USE_COLON | USE_NOSPACE);
        n0 += ocdwarf_printf_ADDR(p, fde_item->hi_pc, USE_TBRT | USE_NOSPACE);

        // <_getchar>
        n0 += printf_text(name, USE_LT | USE_TB);

        // <cie offset 0x00000034::cie index 0>
        n0 += printf_text("cie offset", USE_LT | USE_TBLT);
        n0 += ocdwarf_printf_ADDR(p, fde_item->cie_offset, USE_NONE);
        n0 += printf_text("::cie index", USE_LT);
        n0 += printf_nice(fde_item->cie_index, USE_DEC | USE_TBRT);

        // <fde offset 0x00000030::length 0x00000024>
        n0 += printf_text("fde offset", USE_LT | USE_TBLT);
        n0 += ocdwarf_printf_ADDR(p, fde_item->fde_offset, USE_NONE);
        n0 += printf_text("::length", USE_LT);
        n0 += printf_nice(fde_item->fde_bytes_length, USE_FHEX32 | USE_TBRT);
        n0 += printf_eol();
      } else {
        n0 += printf_nice(fde_item->fde_offset, USE_LHEX32 | USE_NOSPACE);
        n0 += printf_nice(fde_item->fde_bytes_length, USE_LHEXNN);
        n0 += printf_nice(fde_item->cie_offset, USE_LHEX32);
        n0 += printf_text("FDE cie=", USE_LT | USE_SPACE);
        n0 += printf_nice(fde_item->cie_index, USE_LHEX32 | USE_NOSPACE);
        n0 += printf_text("pc=", USE_LT | USE_SPACE);
        n0 += printf_nice(fde_item->lo_pc, USE_LHEXNN | USE_NOSPACE);
        n0 += printf_text("..", USE_LT);
        n0 += printf_nice(fde_item->hi_pc, USE_LHEXNN | USE_NOSPACE);
        n0 += printf_eol();
      }

      Dwarf_Small *augdata = 0;
      Dwarf_Unsigned augdata_len = 0;
      x = dwarf_get_fde_augmentation_data(fde_item->fde, &augdata, &augdata_len, e);
      if (IS_DLV_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
        printf_e("dwarf_get_fde_augmentation_data failed! - %d", x);
#endif
        return OCDWARF_ERRCODE(x, n0);
      }

      if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE | OPTDWARF_ENHANCED)) {
        n0 += printf_text("eh aug data len", USE_LT | USE_SPACE | USE_TBLT);
        if (0 == augdata_len) {
          n0 += printf_nice(augdata_len, USE_FHEX | USE_TBRT | USE_EOL);
        } else {
          n0 += printf_nice(augdata_len, USE_FHEX);
          n0 += printf_hurt(augdata, augdata_len, USE_HEX | USE_SPACE | USE_0x | USE_TBRT | USE_EOL);
        }
      }

      MALLOCA(bool_t, isneeded, 100);
      for (Dwarf_Addr j = fde_item->lo_pc; j < fde_item->hi_pc; ++j) {
        Dwarf_Addr     curr_pc = j;
        Dwarf_Addr     row_pc = 0;
        Dwarf_Addr     subsequent_pc = 0;
        Dwarf_Bool     has_more_rows = 0;
        Dwarf_Small    value_type = 0;
        Dwarf_Block    block = ZEROBLOCK;
        Dwarf_Signed   curr_offset = 0;
        Dwarf_Unsigned offset_relevant = 0;
        Dwarf_Unsigned reg = 0;

        x = dwarf_get_fde_info_for_cfa_reg3_c(fde_item->fde, curr_pc, &value_type, &offset_relevant,
                     &reg, &curr_offset, &block, &row_pc, &has_more_rows, &subsequent_pc, e);
        if (IS_DLV_NO_ENTRY(x)) continue;
        else if (IS_DLV_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
          printf_e("dwarf_get_fde_info_for_cfa_reg3_c failed! - %d", x);
#endif
          return OCDWARF_ERRCODE(x, n0);
        }

        if (DW_EXPR_EXPRESSION != value_type && DW_EXPR_VAL_EXPRESSION != value_type) {
          if (isused(get_REGUSE(p), reg)) {
            isneeded[reg] = TRUE;
          }
        }

        if (!has_more_rows) {
          j = fde_item->hi_pc - 1;
        } else if (subsequent_pc > j) {
          j = subsequent_pc - 1;
        }

        for (Dwarf_Half curr_reg = 0; curr_reg < 100; ++curr_reg) {
          Dwarf_Addr     row_pc = 0;
          Dwarf_Addr     subsequent_pc = 0;
          Dwarf_Bool     has_more_rows = 0;
          Dwarf_Small    value_type = 0;
          Dwarf_Block    block = ZEROBLOCK;
          Dwarf_Signed   offset = 0;
          Dwarf_Unsigned offset_relevant = 0;
          Dwarf_Unsigned reg = 0;

          x = dwarf_get_fde_info_for_reg3_c(fde_item->fde, curr_reg, curr_pc, &value_type, &offset_relevant,
                     &reg, &offset, &block, &row_pc, &has_more_rows, &subsequent_pc, e);
          if (IS_DLV_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
            printf_e("dwarf_get_fde_info_for_reg3_c failed! - %d", x);
#endif
            return OCDWARF_ERRCODE(x, n0);
          } else if (IS_DLV_NO_ENTRY(x) || row_pc != curr_pc || (0 == value_type && 0 == offset)) continue;

          if (isused(get_REGUSE(p), curr_reg)) {
            isneeded[curr_reg] = TRUE;
          }
        }
      }

      if (MODE_ISFIX(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED, OPTDWARF_ENHANCED)) {
        n0 += printf_text("LOC", USE_LT | USE_TAB | SET_PAD(MAXSIZENN));
        n0 += printf_text("CFA", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));

        if (EM_ARM == ocget_machine(p)) {
          printf_w("TBD - missing ARM32 functionality.");
        } if (EM_AARCH64 == ocget_machine(p)) {
          if (isneeded[REG_X19])   n0 += printf_text("x19", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
          if (isneeded[REG_X20])   n0 += printf_text("x20", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
          if (isneeded[REG_X21])   n0 += printf_text("x21", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
          if (isneeded[REG_X22])   n0 += printf_text("x22", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
          if (isneeded[REG_X23])   n0 += printf_text("x23", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
          if (isneeded[REG_X24])   n0 += printf_text("x24", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
          if (isneeded[REG_X29])   n0 += printf_text("x29", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
        } else if (ocisELF32(p)) {
          if (EM_RISCV == ocget_machine(p)) {
            printf_w("TBD - missing RISC32 functionality.");
          } else {
            if (isneeded[REG_EAX])   n0 += printf_text("eax", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (isneeded[REG_ECX])   n0 += printf_text("ecx", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (isneeded[REG_EDX])   n0 += printf_text("edx", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (isneeded[REG_EBX])   n0 += printf_text("ebx", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (isneeded[REG_EBP])   n0 += printf_text("ebp", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (isneeded[REG_ESI])   n0 += printf_text("esi", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (isneeded[REG_EDI])   n0 += printf_text("edi", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
          }
        } else if (ocisELF64(p)) {
          if (EM_RISCV == ocget_machine(p)) {
            printf_w("TBD - missing RISC64 functionality.");
          } else {
            if (isneeded[REG_RBX])   n0 += printf_text("rbx", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (isneeded[REG_RBP])   n0 += printf_text("rbp", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (isneeded[REG_R12])   n0 += printf_text("r12", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (isneeded[REG_R13])   n0 += printf_text("r13", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (isneeded[REG_R14])   n0 += printf_text("r14", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (isneeded[REG_R15])   n0 += printf_text("r15", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
          }
        }

        n0 += printf_text("ra", USE_LT | USE_SPACE);
        n0 += printf_eol();
      }

      for (Dwarf_Addr j = fde_item->lo_pc; j < fde_item->hi_pc; ++j) {
        Dwarf_Addr     curr_pc = j;
        Dwarf_Addr     row_pc = 0;
        Dwarf_Addr     subsequent_pc = 0;
        Dwarf_Bool     has_more_rows = 0;
        Dwarf_Small    value_type = 0;
        Dwarf_Block    block = ZEROBLOCK;
        Dwarf_Signed   curr_offset = 0;
        Dwarf_Unsigned offset_relevant = 0;
        Dwarf_Unsigned reg = 0;

        x = dwarf_get_fde_info_for_cfa_reg3_c(fde_item->fde, curr_pc, &value_type, &offset_relevant,
                     &reg, &curr_offset, &block, &row_pc, &has_more_rows, &subsequent_pc, e);
        if (IS_DLV_NO_ENTRY(x)) continue;
        else if (IS_DLV_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
          printf_e("dwarf_get_fde_info_for_cfa_reg3_c failed! - %d", x);
#endif
          return OCDWARF_ERRCODE(x, n0);
        }

        if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
          n0 += ocdwarf_printf_ADDR(p, j, USE_COLON);
          n0 += ocdwarf_printf_EXPR(p, value_type, USE_SPACE | USE_TBLT);
          n0 += printf_text("cfa=", USE_LT | USE_SPACE);
          if (DW_EXPR_EXPRESSION == value_type || DW_EXPR_VAL_EXPRESSION == value_type) {
            n0 += printf_text("expr-block-len=", USE_LT);
            n0 += printf_nice(block.bl_len, USE_DEC | USE_NOSPACE);
          } else {
            n0 += printf_nice(curr_offset, USE_DEC2Z | USE_NOSPACE);
            n0 += printf_join("r", reg, USE_DEC | USE_RB);
          }
          n0 += printf_stop(USE_TBRT);
        } else if (MODE_ISANY(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED)) {
          n0 += printf_nice(curr_pc, USE_LHEXNN | USE_NOSPACE);

          if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
            n0 += ocdwarf_printf_EXPR(p, value_type, USE_SPACE | USE_TB);
          }

          if (DW_EXPR_EXPRESSION == value_type || DW_EXPR_VAL_EXPRESSION == value_type) {
            n0 += printf_text("exp", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
              n0 += printf_hurt(block.bl_data, block.bl_len, USE_HEX | USE_SPACE | USE_TB);
            }
          } else {
            int n1 = 0;
            n1 += ocdwarf_printf_REGISTER(p, reg, USE_NONE);
            n1 += printf_text("+", USE_LT);
            n1 += printf_nice(curr_offset, USE_DEC | USE_NOSPACE);
            n0 += printf_pack(MAXSIZE - n1) + n1;
          }
        }

        if (!has_more_rows) {
          j = fde_item->hi_pc - 1;
        } else if (subsequent_pc > j) {
          j = subsequent_pc - 1;
        }

        for (Dwarf_Half curr_reg = 0; curr_reg < 100; ++curr_reg) {
          Dwarf_Addr     row_pc = 0;
          Dwarf_Addr     subsequent_pc = 0;
          Dwarf_Bool     has_more_rows = 0;
          Dwarf_Small    value_type = 0;
          Dwarf_Block    block = ZEROBLOCK;
          Dwarf_Signed   offset = 0;
          Dwarf_Unsigned offset_relevant = 0;
          Dwarf_Unsigned reg = 0;

          x = dwarf_get_fde_info_for_reg3_c(fde_item->fde, curr_reg, curr_pc, &value_type, &offset_relevant,
                     &reg, &offset, &block, &row_pc, &has_more_rows, &subsequent_pc, e);
          if (IS_DLV_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
            printf_e("dwarf_get_fde_info_for_reg3_c failed! - %d", x);
#endif
            return OCDWARF_ERRCODE(x, n0);
          } else if (IS_DLV_NO_ENTRY(x) || row_pc != curr_pc || (0 == value_type && 0 == offset)) {
            if (MODE_ISFIX(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED, OPTDWARF_ENHANCED)) {
              if (isneeded[curr_reg]) {
                n0 += printf_text("u", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
//printf(" u(%d)", curr_reg);
              }
            }
            continue;
          }

          if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
            n0 += ocdwarf_printf_EXPR(p, value_type, USE_TBLT);
            n0 += printf_join("r", curr_reg, USE_DEC | USE_SPACE);
            n0 += printf_text("=", USE_LT);
            if (DW_EXPR_EXPRESSION == value_type || DW_EXPR_VAL_EXPRESSION == value_type) {
              n0 += printf_text("expr-block-len=", USE_LT);
              n0 += printf_nice(block.bl_len, USE_DEC | USE_NOSPACE);
            } else {
              n0 += printf_nice(offset, USE_DEC | USE_NOSPACE);
              n0 += printf_text("cfa", USE_LT | USE_RB);
            }
            n0 += printf_stop(USE_TBRT);
          } else if (MODE_ISANY(oc->ocdump, OPTDWARF_DEBUG_FRAME_DECODED)) {
            if (MODE_ISANY(oc->ocdump, OPTDWARF_VERBOSE)) {
              n0 += ocdwarf_printf_EXPR(p, value_type, USE_SPACE | USE_TB);
            }

            if (DW_EXPR_EXPRESSION == value_type || DW_EXPR_VAL_EXPRESSION == value_type) {
              n0 += printf_text("exp", USE_LT | USE_SPACE | SET_PAD(MAXSIZE));
            } else if (DW_EXPR_OFFSET == value_type || DW_EXPR_VAL_OFFSET == value_type) {
//printf(" r(%d)", curr_reg);
              int n1 = 0;
              n1 += printf_text("c", USE_LT | USE_SPACE);
              n1 += printf_nice(offset, USE_DEC | USE_NOSPACE);
              n0 += printf_pack(MAXSIZE - n1) + n0;
            }
          }
        }

        n0 += printf_eol();
      }

      n0 += printf_eol();
    }

    if (MODE_ISNOT(oc->ocdump, OPTDWARF_ENHANCED)) {
      fde_item = fde_items + fde_count -1;
      n0 += printf_nice(fde_item->cie_offset + fde_item->fde_bytes_length, USE_LHEX32);
      n0 += printf_text("ZERO terminator", USE_LT | USE_SPACE);
      n0 += printf_eol();
    }

    xfree(fde_items);
  }

  return OCDWARF_ERRCODE(x, n0);
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
    if (IS_DLV_NO_ENTRY(x)) {
      if (MODE_ISNOT(oc->ocdump, OPTDWARF_ENHANCED)) {
        n += printf_nice(0x0, USE_LHEX32);
        n += printf_text("ZERO terminator", USE_LT | USE_SPACE);
        n += printf_eol();
      }

      return n;
    } else if (IS_DLV_ERROR(x)) {
#ifdef OPCODE_DWARF_DEBUGX
      printf_e("dwarf_get_fde_list_eh failed! - %d", x);
#endif
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

    if (MODE_ISANY(oc->ocdump, OPTDWARF_ENHANCED)) {
      n += ocdwarf_printf_groups(p, ocget(p, OPCODE_DWARF_ERROR));
      n += ocdwarf_eh_frame_fdes(p, fde_data, fde_element_count, ocget(p, OPCODE_DWARF_ERROR));
      n += ocdwarf_eh_frame_cies(p, cie_data, cie_element_count, ocget(p, OPCODE_DWARF_ERROR));
    } else {
      n += ocdwarf_eh_frame_cies(p, cie_data, cie_element_count, ocget(p, OPCODE_DWARF_ERROR));
      n += ocdwarf_eh_frame_fdes(p, fde_data, fde_element_count, ocget(p, OPCODE_DWARF_ERROR));
    }

    ocdwarf_dealloc_fde_cie_list(p, cie_data, cie_element_count, fde_data, fde_element_count);
  }

  return OCDWARF_ERRCODE(x, n);
}

