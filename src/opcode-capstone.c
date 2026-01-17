#include "opcode.h"
#include "printf.h"
#include "elfcode.h"
#include "ocdwarf.h"
#include "options.h"
#include "opcode-printf.h"
#include "opcode-capstone.h"

static int get_csarch(handle_t p, handle_t o) {
  if (isoptions(o)) {
    poptions_t op = CAST(poptions_t, o);
    if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_X86_64 | OPTDISASSEMBLE_I386 | OPTDISASSEMBLE_I8086)) {
      return CS_ARCH_X86;
    } else if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_ARM32)) {
      return CS_ARCH_ARM;
    } else if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_ARM64)) {
      return CS_ARCH_AARCH64;
    } else if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_RISCV32 | OPTDISASSEMBLE_RISCV64)) {
      return CS_ARCH_RISCV;
    }
  }

  if (EM_ARM == ocget_machine(p)) {
    return CS_ARCH_ARM;
  } else if (EM_AARCH64 == ocget_machine(p)) {
    return CS_ARCH_AARCH64;
  } else if (EM_RISCV == ocget_machine(p)) {
    return CS_ARCH_RISCV;
  }

  return CS_ARCH_X86;
}

static int get_csmode(handle_t p, handle_t o) {
  if (isoptions(o)) {
    poptions_t op = CAST(poptions_t, o);
    if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_X86_64))        return CS_MODE_64;
    else if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_I386))     return CS_MODE_32;
    else if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_I8086))    return CS_MODE_16;
    else if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_ARM32))    return CS_MODE_ARM;
    else if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_ARM64))    return ocisLE(p) ? CS_MODE_LITTLE_ENDIAN : CS_MODE_BIG_ENDIAN;
    else if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_RISCV64))  return CS_MODE_RISCV64;
    else if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_RISCV32))  return CS_MODE_RISCV32;
  }

  const uint64_t mach = ocget_machine(p);
  const uint64_t size = ocget_archsize(p);

  if (EM_ARM == mach) {
    return CS_MODE_ARM;
  } else if (EM_AARCH64 == mach) {
    return ocisLE(p) ? CS_MODE_LITTLE_ENDIAN : CS_MODE_BIG_ENDIAN;
  } else if (EM_RISCV == mach) {
    if (32 == size) return CS_MODE_RISCV32;
    else if (64 == size) return CS_MODE_RISCV64;
  } else {
    if (16 == size) return CS_MODE_16;
    else if (32 == size) return CS_MODE_32;
  }

  return CS_MODE_64;
}

int capstone_open(handle_t p, handle_t o) {
  if (isopcode(p) && isoptions(o)) {
    poptions_t op = CAST(poptions_t, o);
    popcode_t oc = ocget(p, OPCODE_THIS);
    if (CS_ERR_OK == cs_open(get_csarch(p, o), get_csmode(p, o), &oc->cs)) {
      if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_ATT | OPTDISASSEMBLE_ATT_MNEMONIC)) {
        cs_option(oc->cs, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);
      } else if (MODE_ISANY(op->ocdump, OPTDISASSEMBLE_INTEL | OPTDISASSEMBLE_INTEL_MNEMONIC)) {
        cs_option(oc->cs, CS_OPT_SYNTAX, CS_OPT_SYNTAX_INTEL);
      } else {
        cs_option(oc->cs, CS_OPT_SYNTAX, CS_OPT_SYNTAX_ATT);
      }

//      cs_option(oc->cs, CS_OPT_DETAIL, CS_OPT_ON);
//      cs_option(oc->cs, CS_OPT_SKIPDATA, CS_OPT_ON);
      return ECODE_OK;
    }

    return ECODE_CAPSTONE;
  }

  return ECODE_HANDLE;
}

int capstone_close(handle_t p) {
  if (isopcode(p)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    cs_close(&oc->cs);
    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int capstone_raw0(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr) {
  int n = 0;
  if (data && isopcode(p) && ismodeNXXN(s, MODE_OCSHDRWRAP)) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    cs_insn *insn = NULL;
    size_t count = cs_disasm(oc->cs, data, size, vaddr, 0, &insn);
    if (count > 0) {
      bool_t bskip = FALSE;
      uchar_t iskip = 0;
      for (size_t i = 0; i < count; ++i) {
        if (ocuse_vaddr(p, insn[i].address)) {
          int n1 = 0;
          int n2 = 0;
          if (bskip && iskip != insn[i].bytes[0]) {
            n1 += printf_text(">>>>>>>>", USE_LT | USE_SPACE | USE_COLON);
            n1 += printf_eol();
            bskip = FALSE;
          }
          if (!bskip) {
            n2 += opcode_printf_source(p, insn[i].address);

            if (MODE_ISNOT(oc->action, OPTPROGRAM_NO_ADDRESSES)) {
              n2 += opcode_printf_LHEX(p, insn[i].address, USE_COLON);
            }

            if (MODE_ISANY(oc->action, OPTPROGRAM_PREFIX_ADDR)) {
              n2 += opcode_printf_prefix(p, insn[i].address);
            } else if (MODE_ISNOT(oc->action, OPTPROGRAM_NO_SHOW_RAW_INSN)) {
              n2 += printf_sore(insn[i].bytes, insn[i].size, USE_HEX | USE_SPACE);
              n2 += printf_pack(42 - n2);
            }

            n2 += opcode_printf_detail(p, insn[i].address, insn[i].mnemonic, insn[i].op_str);
            n2 += printf_eol();
          }
          if (!ocuse_insn(p, insn[i].bytes[0])) {
            iskip = insn[i].bytes[0];
            bskip = TRUE;
          }
          n += n1 + n2;
        }
      }

      cs_free(insn, count);
    } else {
      printf_e("Failed to disassemble given code!");
    }
  }

  return n;
}

static uchar_t capstone_check(pthumb_t thumbs, const size_t maxthumbs, const uint64_t vaddr) {
  if (thumbs && maxthumbs) {
    pthumb_t p0 = thumbs;

    for (size_t i = 0; i < maxthumbs; ++i, ++p0) {
      if (vaddr == p0->vaddr) {
//printf("%lx:%c\n", p0->vaddr, p0->value);
        return p0->value;
      }
    }
  }

  return 0;
}

int capstone_raw1(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr) {
  int n = 0;
  if (data && isopcode(p) && ismodeNXXN(s, MODE_OCSHDRWRAP)) {
    MALLOCA(thumb_t, thumbs, 1024);
    size_t siz = ecmake_sectionthumbs(ocget(p, OPCODE_RAWDATA), thumbs, NELEMENTS(thumbs));

    popcode_t oc = ocget(p, OPCODE_THIS);
    puchar_t p0 = CAST(puchar_t, data);
    uint64_t caddr = vaddr;
    size_t   caddrsize = 4;

    char prev_state = 'a';
    for (size_t k = 0; k < size; ) {
      char curr_state = capstone_check(thumbs, siz, caddr);
      curr_state = curr_state ? curr_state : prev_state;
//printf("<--- %lx|%c\n", caddr, curr_state);

      if ('d' == curr_state) {
        if (ocuse_vaddr(p, caddr)) {
          int n1 = 0;
          if (MODE_ISNOT(oc->action, OPTPROGRAM_NO_ADDRESSES)) {
            n1 += opcode_printf_LHEX(p, caddr, USE_COLON);
          }

          if (MODE_ISANY(oc->action, OPTPROGRAM_PREFIX_ADDR)) {
            n1 += opcode_printf_prefix(p, caddr);
          } else if (MODE_ISNOT(oc->action, OPTPROGRAM_NO_SHOW_RAW_INSN)) {
            n1 += printf_sore(p0, caddrsize, USE_HEX | USE_SPACE);
            n1 += printf_pack(42 - n1);
          }

          n += n1;
          n += printf_text(".word", USE_LT | USE_SPACE);
          n += printf_nice(ocmake_u32(p, p0[0], p0[1], p0[2], p0[3]), USE_FHEX32);
          n += printf_eol();
        }

        k += caddrsize;
        p0 += caddrsize,
        caddr += caddrsize;
      } else {
        if (curr_state != prev_state) {
          if (CS_ERR_OK == cs_close(&oc->cs)) {
            cs_open(CS_ARCH_ARM, 't' == curr_state ? CS_MODE_THUMB : CS_MODE_ARM, &oc->cs);
          }
        }

        cs_insn *insn = NULL;
        size_t count = cs_disasm(oc->cs, p0, caddrsize, caddr, 0, &insn);
        if (count > 0) {
          for (size_t i = 0; i < count; ++i) {
            if (ocuse_vaddr(p, insn[i].address)) {
              int n1 = 0;

              char next_state = capstone_check(thumbs, siz, caddr);
              next_state = next_state ? next_state : prev_state;
              if (next_state != curr_state) break;

              n1 += opcode_printf_source(p, insn[i].address);

              if (MODE_ISNOT(oc->action, OPTPROGRAM_NO_ADDRESSES)) {
                n1 += opcode_printf_LHEX(p, insn[i].address, USE_COLON);
              }

              if (MODE_ISANY(oc->action, OPTPROGRAM_PREFIX_ADDR)) {
                n1 += opcode_printf_prefix(p, insn[i].address);
              } else if (MODE_ISNOT(oc->action, OPTPROGRAM_NO_SHOW_RAW_INSN)) {
                n1 += printf_sore(insn[i].bytes, insn[i].size, USE_HEX | USE_SPACE);
                n1 += printf_pack(42 - n1);
              }

              n += n1;
              n += opcode_printf_detail(p, insn[i].address, insn[i].mnemonic, insn[i].op_str);
              n += printf_eol();
            }

            k += insn[i].size;
            p0 += insn[i].size,
            caddr += insn[i].size;
          }
        } else {
          k += caddrsize;
          p0 += caddrsize,
          caddr += caddrsize;
        }
      }

      prev_state = curr_state;
    }

    if (CS_ERR_OK == cs_close(&oc->cs)) {
      cs_open(CS_ARCH_ARM, CS_MODE_ARM, &oc->cs);
    }
  }

  return n;
}

int capstone_raw2(handle_t p, handle_t s, unknown_t data, const size_t size, const uint64_t vaddr) {
  int n = 0;
  if (data && isopcode(p) && ismodeNXXN(s, MODE_OCSHDRWRAP)) {
    popcode_t oc = ocget(p, OPCODE_THIS);
    puchar_t p0 = CAST(puchar_t, data);

    uint64_t caddr = vaddr;
    int core_state = get_csmode(p, NULL);
    int prev_state = core_state;

    for (size_t k = 0; k < size; ) {
      cs_insn *insn = NULL;
      size_t count = cs_disasm(oc->cs, p0, 4, caddr, 0, &insn);
      if (count > 0) {
        for (size_t i = 0; i < count; ++i) {
            if (ocuse_vaddr(p, insn[i].address)) {
              int n1 = 0;

              n1 += opcode_printf_source(p, insn[i].address);

              if (MODE_ISNOT(oc->action, OPTPROGRAM_NO_ADDRESSES)) {
                n1 += opcode_printf_LHEX(p, insn[i].address, USE_COLON);
              }

              if (MODE_ISANY(oc->action, OPTPROGRAM_PREFIX_ADDR)) {
                n1 += opcode_printf_prefix(p, insn[i].address);
              } else if (MODE_ISNOT(oc->action, OPTPROGRAM_NO_SHOW_RAW_INSN)) {
                n1 += printf_sore(insn[i].bytes, insn[i].size, USE_HEX | USE_SPACE);
                n1 += printf_pack(42 - n1);
              }

              n += n1;
              n += opcode_printf_detail(p, insn[i].address, insn[i].mnemonic, insn[i].op_str);
              n += printf_eol();
            }

          k += insn[i].size;
          p0 += insn[i].size,
          caddr += insn[i].size;
        }
      }
    }

    if (core_state != prev_state && CS_ERR_OK == cs_close(&oc->cs)) {
      cs_open(CS_ARCH_RISCV, core_state, &oc->cs);
    }
  }

  return n;
}

int capstone_run(handle_t p, handle_t s) {
  int n = 0;
  if (isopcode(p) && isopshdr(s)) {
    const size_t sz = ocget_size(s);
    unknown_t    p0 = xmalloc(sz);

    if (p0) {
      if (bfd_get_section_contents(ocget(p, OPCODE_BFD), ocget(s, MODE_OCSHDR), p0, 0, sz)) {
//        n += capstone_raw2(p, s, p0, sz, ocget_vmaddress(s));
        if (ecmake_sectionthumbs(ocget(p, OPCODE_RAWDATA), NULL, 0)) {
          n += capstone_raw1(p, s, p0, sz, ocget_vmaddress(s));
        } else {
          n += capstone_raw0(p, s, p0, sz, ocget_vmaddress(s));
        }
      }

      xfree(p0);
    }
  } else if (isopcode(p) && ismodeNXXN(s, MODE_OCSHDRWRAP)) {
    const size_t sz = ocget_size(s);
    if (0 != sz) {
      n += capstone_raw0(p, s, getp(ocget(p, OPCODE_RAWDATA), ocget_offset(s), sz), sz, ocget_vmaddress(s));
    }
  }

  return n;
}

