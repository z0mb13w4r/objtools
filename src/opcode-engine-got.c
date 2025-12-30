#include "opcode-engine-got.h"

static handle_t execute_new(handle_t p, const uint64_t vaddr, const char* name) {
  MALLOCA(char, oname, 1024);

  if (isoengine(p) && name) {
    pocgroups_t g0 = oegetbyaddr(p, vaddr, OPENGINE_GROUP);
    if (g0) {
      if (NULL == g0->symbol) {
        g0->symbol = osmalloc(vaddr);
      }

      int epos = snprintf(oname, sizeof(oname), "%s", name);
      char *cpos = strchr(oname, '@');
      if (cpos) {
        epos = cpos - oname;
      }
      snprintf(oname + epos, sizeof(oname) - epos, "@plt");

      g0->symbol->name = xstrdup(oname);
      g0->symbol->role |= OPSYMBOL_NAME;
    }
  }

  return p;
}

static uint32_t execute_u32(handle_t p, const uchar_t v0, const uchar_t v1, const uchar_t v2, const uchar_t v3) {
  return v0 | (v1 << 8) | (v2 << 16) | (v3 << 24);
}

static void execute_section32arm(handle_t p, handle_t s, handle_t q) {
  puchar_t pp = ocget_rawdata(s);
  if (pp) {
    execute_new(q, 0x0000054c, "strcmp");
    execute_new(q, 0x00000558, "__cxa_finalize");
    execute_new(q, 0x00000564, "read");
    execute_new(q, 0x00000570, "__stack_chk_fail");
    execute_new(q, 0x0000057c, "strcpy");
    execute_new(q, 0x00000588, "puts");
    execute_new(q, 0x00000594, "malloc");
    execute_new(q, 0x000005a0, "__libc_start_main");
    execute_new(q, 0x000005ac, "__gmon_start__");
    execute_new(q, 0x000005b8, "__ctype_b_loc");
    execute_new(q, 0x000005c4, "strlen");
    execute_new(q, 0x000005d0, "__printf_chk");
    execute_new(q, 0x000005dc, "abort");

    uint64_t curr_vaddr = ocget_vmaddress(s);
    for (uint64_t i = 0; i < ocget_size(s); ) {
      uint64_t siz = 1;

      i += siz;
      curr_vaddr += siz;
    }
  }
}

static void execute_section32x86(handle_t p, handle_t s, handle_t q) {
  puchar_t pp = ocget_rawdata(s);
  if (pp) {
    uint64_t curr_vaddr = ocget_vmaddress(s);
    uint64_t prev_vaddr0 = 0;
    uint32_t prev_vaddr1 = 0;
    uint64_t prev_vaddr2 = ocget_vaddressbyname(p, ".got");
    for (uint64_t i = 0; i < ocget_size(s); ) {
      uint64_t siz = 1;

      if (0xf3 == pp[i + 0] && 0x0f == pp[i + 1] && 0x1e == pp[i + 2] && 0xfb == pp[i + 3]) { // endbr32
        prev_vaddr0 = curr_vaddr;
        siz = 4;
      } else if (0xff == pp[i + 0] && 0xa3 == pp[i + 1]) { // jmp
        prev_vaddr1 = execute_u32(p, pp[i + 2], pp[i + 3], pp[i + 4], pp[i + 5]);
        siz = 6;
      } else if (0x66 == pp[i + 0] && 0x0f == pp[i + 1] && 0x1f == pp[i + 2] && 0x44 == pp[i + 3] && 0x00 == pp[i + 4] && 0x00 == pp[i + 5]) { // nopw
        uint64_t this_vaddr = prev_vaddr2 + prev_vaddr1;
        execute_new(q, prev_vaddr0, ocget_namebyvaddr(p, this_vaddr, NULL));
        siz = 6;
      }

      i += siz;
      curr_vaddr += siz;
    }
  }
}

// DDI0487_M_a_a_a-profile_architecture_reference_manual.pdf
//                     10987654321098765432109876543210
static char zADD[]  = "x00100010Siiiiiiiiiiiinnnnnddddd"; // C6.2.5 ADD (immediate)
static char zADRP[] = "1II10000iiiiiiiiiiiiiiiiiiiddddd"; // C6.2.13 ADRP
static char zBR[]   = "1101011000011111000000nnnnnmmmmm"; // C6.2.46 BR
static char zLDR[]  = "1s11100101iiiiiiiiiiiinnnnnttttt"; // C6.2.215 LDR (immediate) unsigned offset
static char zSTP[]  = "x010100110iiiiiiiTTTTTnnnnnttttt"; // C6.2.413 STP pre-index

static uint32_t is00(handle_t p, const char* x, const size_t size, const int c, const uint32_t v) {
  if (x && 32 == size) {
    uint32_t xx = 0;
    for (size_t i = 0; i < size; ++i) {
      if (c == x[i]) {
        uint32_t mask = (1 << (31 - i));
        if ('1' == c) {
          if (0 == (v & mask)) return 0;
          xx |= mask;
        } else if ('0' == c) {
          if (0 != (v & mask)) return 0;
          xx |= mask;
        } else if (v & mask) {
          xx |= mask;
        }
      }
    }

    return xx;
  }

  return 0;
}

static uint32_t is01(handle_t p, const char* x, const size_t size, const uint32_t v) {
  return is00(p, x, size, '1', v) && is00(p, x, size, '0', v);
}

static void execute_section64arm(handle_t p, handle_t s, handle_t q) {
  puchar_t pp = ocget_rawdata(s);
  if (pp) {
    uint64_t curr_vaddr = ocget_vmaddress(s);
    uint64_t prev_vaddr0 = 0;
    uint32_t prev_vaddr1 = 0;
    uint32_t prev_vaddr2 = 0;

    for (uint64_t i = 0; i < ocget_size(s); i += 4, curr_vaddr += 4) {
      uint32_t xx = execute_u32(p, pp[i + 0], pp[i + 1], pp[i + 2], pp[i + 3]);
//printf("%03lx:%08x ", curr_vaddr, xx);
//printf("%s", is01(s, zBR,   sizeof(zBR)   - 1, xx) ? "br"   : "");
//printf("%s", is01(s, zADD,  sizeof(zADD)  - 1, xx) ? "add"  : "");
//printf("%s", is01(s, zLDR,  sizeof(zLDR)  - 1, xx) ? "ldr"  : "");
//printf("%s", is01(s, zSTP,  sizeof(zSTP)  - 1, xx) ? "stp"  : "");
//printf("%s", is01(s, zADRP, sizeof(zADRP) - 1, xx) ? "adrp" : "");

      if (is01(s, zSTP, sizeof(zSTP) - 1, xx)) { //stp x16, x30, [sp, #-0x??]!
//        const uint32_t im = is00(s, zSTP, sizeof(zSTP) - 1, 'i', xx) >> 15;
//        const uint32_t RT = is00(s, zSTP, sizeof(zSTP) - 1, 'T', xx) >> 10;
//        const uint32_t Rn = is00(s, zSTP, sizeof(zSTP) - 1, 'n', xx) >> 5;
//        const uint32_t Rt = is00(s, zSTP, sizeof(zSTP) - 1, 't', xx);
//printf("|%x", is00(s, zSTP, sizeof(zSTP) - 1, 'I', xx));
//printf("|imm=%x", im);
//printf("|RT=%x:x%d", RT, RT);
//printf("|Rn=%x:x%d", Rn, Rn);
//printf("|Rt=%x:x%d", Rt, Rt);
      } else if (is01(s, zADRP, sizeof(zADRP) - 1, xx)) { // adrp x16, 0x?????
        const uint32_t lo = is00(s, zADRP, sizeof(zADRP) - 1, 'I', xx);
        const uint32_t hi = is00(s, zADRP, sizeof(zADRP) - 1, 'i', xx);
//        const uint32_t Rd = is00(s, zADRP, sizeof(zADRP) - 1, 'd', xx);

        prev_vaddr0 = curr_vaddr;
        prev_vaddr1 = ((lo >> 29) | (hi >> 3)) << 12;
//printf("|lo=%x|hi=%x|imm=%x|Rd=%x:x%d", lo, hi, prev_vaddr1, Rd, Rd);
      } else if (is01(s, zBR, sizeof(zBR) - 1, xx)) { // br x17
//        const uint32_t Rn = is00(s, zBR, sizeof(zBR) - 1, 'n', xx) >> 5;
//printf("|Rn=%x:x%d", Rn, Rn);
        execute_new(q, prev_vaddr0, ocget_namebyvaddr(p, prev_vaddr1 + prev_vaddr2, NULL));
      } else if (is01(s, zLDR, sizeof(zLDR) - 1, xx)) { // ldr x17, [x16, #0x???]
        prev_vaddr2       = is00(s, zLDR, sizeof(zLDR) - 1, 'i', xx) >> 7;
//        const uint32_t Rn = is00(s, zLDR, sizeof(zLDR) - 1, 'n', xx) >> 5;
//        const uint32_t Rt = is00(s, zLDR, sizeof(zLDR) - 1, 't', xx);
//printf("|%x", is00(s, zLDR, sizeof(zLDR) - 1, 's', xx));
//printf("|imm=%x", prev_vaddr2);
//printf("|Rn=%x:x%d", Rn, Rn);
//printf("|Rt=%x:x%d", Rt, Rt);
      } else if (is01(s, zADD, sizeof(zADD) - 1, xx)) { // add x16, x16, #0x???
//        const uint32_t im = is00(s, zADD, sizeof(zADD) - 1, 'i', xx) >> 10;
//        const uint32_t Rn = is00(s, zADD, sizeof(zADD) - 1, 'n', xx) >> 5;
//        const uint32_t Rd = is00(s, zADD, sizeof(zADD) - 1, 'd', xx);
//printf("|%x", is00(s, zADD, sizeof(zADD) - 1, 'S', xx));
//printf("|imm=%x", im);
//printf("|Rn=%x:x%d", Rn, Rn);
//printf("|Rd=%x:x%d", Rd, Rd);
      }

//printf("\n");
    }
  }
}

static void execute_section64x86(handle_t p, handle_t s, handle_t q) {
  puchar_t pp = ocget_rawdata(s);
  if (pp) {
    uint64_t curr_vaddr = ocget_vmaddress(s);
    uint64_t prev_vaddr0 = 0;
    uint32_t prev_vaddr1 = 0;
    for (uint64_t i = 0; i < ocget_size(s); ) {
      uint64_t siz = 1;
      if (0xf3 == pp[i + 0] && 0x0f == pp[i + 1] && 0x1e == pp[i + 2] && 0xfa == pp[i + 3]) { // endbr64
        prev_vaddr0 = curr_vaddr;
        siz = 4;
      } else if (0x0f == pp[i + 0] && 0x1f == pp[i + 1] && 0x44 == pp[i + 2] && 0x00 == pp[i + 3] && 0x00 == pp[i + 4]) { // nopl
        uint64_t this_vaddr = curr_vaddr + prev_vaddr1;
        execute_new(q, prev_vaddr0, ocget_namebyvaddr(p, this_vaddr, NULL));
        siz = 5;
      } else if (0x0f == pp[i + 0] && 0x1f == pp[i + 1] && 0x00 == pp[i + 2]) { // nopl
        siz = 3;
      } else if (0xf2 == pp[i + 0] && 0xff == pp[i + 1] && 0x25 == pp[i + 2]) { // bnd jmpq
        prev_vaddr1 = execute_u32(p, pp[i + 3], pp[i + 4], pp[i + 5], pp[i + 6]);
        siz = 7;
      } else if (0xf2 == pp[i + 0] && 0xe9 == pp[i + 1]) { // bnd jmpq
        siz = 6;
      } else if (0xff == pp[i + 0] && 0x35 == pp[i + 1]) { // pushq
        siz = 6;
      } else if (0x68 == pp[i + 0]) {
        siz = 5;
      }

      i += siz;
      curr_vaddr += siz;
    }
  }
}

static void callback_sections32(handle_t p, handle_t shdr, unknown_t param) {
  const char* name = ocget_name(shdr);
  const uint64_t e = ocget_machine(p);
  if (EM_ARM == e) {
    if (0 == xstrcmp(name, ".plt")) {
      execute_section32arm(p, shdr, param);
    }
  } else if (EM_RISCV == e) {
  } else {
    if (0 == xstrcmp(name, ".plt.got")) {
      execute_section32x86(p, shdr, param);
    } else if (0 == xstrcmp(name, ".plt.sec")) {
      execute_section32x86(p, shdr, param);
    }
  }
}

static void callback_sections64(handle_t p, handle_t shdr, unknown_t param) {
  const char* name = ocget_name(shdr);
  const uint64_t e = ocget_machine(p);
  if (EM_AARCH64 == e) {
    if (0 == xstrcmp(name, ".plt")) {
      execute_section64arm(p, shdr, param);
    }
  } else if (EM_RISCV == e) {
  } else {
    if (0 == xstrcmp(name, ".plt.got")) {
      execute_section64x86(p, shdr, param);
    } else if (0 == xstrcmp(name, ".plt.sec")) {
      execute_section64x86(p, shdr, param);
    }
  }
}

handle_t opcode_create(handle_t p, handle_t q) {
  if (isopcode(p) && isoengine(q)) {
    if (ocisELF32(p)) {
      ocdo_sections(p, callback_sections32, q);
    } else if (ocisELF64(p)) {
      ocdo_sections(p, callback_sections64, q);
    }
    return q;
  }

  return NULL;
}

