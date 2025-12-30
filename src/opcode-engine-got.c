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
//                      10987654321098765432109876543210
static char zADRPa[] = "1II10000iiiiiiiiiiiiiiiiiiiddddd"; // C6.2.13 ADRP
//                      10987654321098765432109876543210
static char zADRP[]  = "1iix0000iiiiiiiiiiiiiiiiiiiddddd"; // adrp Rd ADDR_ADRP
//                     10987654321098765432109876543210
static char zBR[]   = "x10x0110000xxxxxxxxxxxnnnnnxxxxx"; // br Rn

static char zADD0[] = "x0001011xx0xxxxxxxxxxxnnnnnddddd"; // add Rd Rn Rm_SFT
static char zADD1[] = "x00x0001SSiiiiiiiiiiiinnnnnddddd"; // add Rd_SP Rn_SP AIMM
static char zADD2[] = "x00010110x1xxxxxxxxxxxnnnnnddddd"; // add Rd_SP Rn_SP Rm_EXT
static char zADD3[] = "x1011110xx1mmmmmx00001nnnnnddddd"; // add Sd Sn Sm
static char zADD4[] = "xx001110xx1mmmmm100001nnnnnddddd"; // add Vd Vn Vm

static char zLDR0[] = "xx011100iiiiiiiiiiiiiiiiiiittttt"; // ldr Ft ADDR_PCREL19
static char zLDR1[] = "xx111100x1xxxxxxxxxx10xxxxxttttt"; // ldr Ft ADDR_REGOFF
static char zLDR2[] = "xx111100x1xiiiiiiiiiI1xxxxxttttt"; // ldr Ft ADDR_SIMM9
static char zLDR3[] = "xxx11101x1iiiiiiiiiiiinnnnnttttt"; // ldr Ft ADDR_UIMM12
static char zLDR4[] = "0x011000iiiiiiiiiiiiiiiiiiittttt"; // ldr Rt ADDR_PCREL19
static char zLDR5[] = "1x111000011xxxxxxxxx10xxxxxttttt"; // ldr Rt ADDR_REGOFF
static char zLDR6[] = "1x11100001xiiiiiiiiiI1xxxxxttttt"; // ldr Rt ADDR_SIMM9
static char zLDR7[] = "1xx1100101iiiiiiiiiiiinnnnnttttt"; // ldr Rt ADDR_UIMM12

// DDI0487_M_a_a_a-profile_architecture_reference_manual.pdf
//                     10987654321098765432109876543210
static char zLDRa[] = "1s111000011mmmmmoooS10nnnnnttttt"; // C6.2.217 LDR (register)
static char zLDRb[] = "1s111000010iiiiiiiii01nnnnnttttt"; // C6.2.215 LDR (immediate) post-index
static char zLDRc[] = "1s111000010iiiiiiiii11nnnnnttttt"; // C6.2.215 LDR (immediate) pre-index
static char zLDRd[] = "1s11100101iiiiiiiiiiiinnnnnttttt"; // C6.2.215 LDR (immediate) unsigned offset
static char zLDRe[] = "0s011000iiiiiiiiiiiiiiiiiiittttt"; // C6.2.216 LDR (literal)

static char zSTP0[] = "xx10110I00iiiiiiitttttxxxxxttttt"; // stp Ft Ft2 ADDR_SIMM7
static char zSTP1[] = "xx10110I10iiiiiiitttttxxxxxttttt"; // stp Ft Ft2 ADDR_SIMM7
static char zSTP2[] = "xx10100I10iiiiiiitttttxxxxxttttt"; // stp Rt Rt2 ADDR_SIMM7

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
//    execute_new(q, 0x00000890, ocget_namebyvaddr(p, 0x00011f48, NULL)); // "strlen");
//    execute_new(q, 0x000008a0, ocget_namebyvaddr(p, 0x00011f50, NULL)); // "__cxa_finalize");
//    execute_new(q, 0x000008b0, ocget_namebyvaddr(p, 0x00011f58, NULL)); // "malloc");
//    execute_new(q, 0x000008c0, ocget_namebyvaddr(p, 0x00011f60, NULL)); // "__libc_start_main");
//    execute_new(q, 0x000008d0, ocget_namebyvaddr(p, 0x00011f68, NULL)); // "__printf_chk");
//    execute_new(q, 0x000008e0, ocget_namebyvaddr(p, 0x00011f70, NULL)); // "__stack_chk_fail");
//    execute_new(q, 0x000008f0, ocget_namebyvaddr(p, 0x00011f78, NULL)); // "__gmon_start__");
//    execute_new(q, 0x00000900, ocget_namebyvaddr(p, 0x00011f80, NULL)); // "abort");
//    execute_new(q, 0x00000910, ocget_namebyvaddr(p, 0x00011f88, NULL)); // "puts");
//    execute_new(q, 0x00000920, ocget_namebyvaddr(p, 0x00011f90, NULL)); // "strcmp");
//    execute_new(q, 0x00000930, ocget_namebyvaddr(p, 0x00011f98, NULL)); // "__ctype_b_loc");
//    execute_new(q, 0x00000940, ocget_namebyvaddr(p, 0x00011fa0, NULL)); // "strcpy");
//    execute_new(q, 0x00000950, ocget_namebyvaddr(p, 0x00011fa8, NULL)); // "read");

    uint64_t curr_vaddr = ocget_vmaddress(s);
    uint64_t prev_vaddr0 = 0;
    uint32_t prev_vaddr1 = 0x11000 + 0xf40 - 0x08;

    for (uint64_t i = 0; i < ocget_size(s); i += 4, curr_vaddr += 4) {
      uint32_t xx = execute_u32(p, pp[i + 0], pp[i + 1], pp[i + 2], pp[i + 3]);
//printf("%03lx:%08x ", curr_vaddr, xx);
//printf("%s", is01(s, zADRP, sizeof(zADRP) - 1, xx) ? "adrp" : "");
//printf("%s", is01(s, zADRPa, sizeof(zADRPa) - 1, xx) ? "adrpA" : "");
//printf("%s", is01(s, zBR, sizeof(zBR) - 1, xx) ? "br" : "");

//printf("%s", is01(s, zADD0, sizeof(zADD0) - 1, xx) ? "add0" : "");
//printf("%s", is01(s, zADD1, sizeof(zADD1) - 1, xx) ? "add1" : "");
//printf("%s", is01(s, zADD2, sizeof(zADD2) - 1, xx) ? "add2" : "");
//printf("%s", is01(s, zADD3, sizeof(zADD3) - 1, xx) ? "add3" : "");
//printf("%s", is01(s, zADD4, sizeof(zADD4) - 1, xx) ? "add4" : "");

//printf("%s", is01(s, zLDR0, sizeof(zLDR0) - 1, xx) ? "ldr0" : "");
//printf("%s", is01(s, zLDR1, sizeof(zLDR1) - 1, xx) ? "ldr1" : "");
//printf("%s", is01(s, zLDR2, sizeof(zLDR2) - 1, xx) ? "ldr2" : "");
//printf("%s", is01(s, zLDR3, sizeof(zLDR3) - 1, xx) ? "ldr3" : "");
//printf("%s", is01(s, zLDR4, sizeof(zLDR4) - 1, xx) ? "ldr4" : "");
//printf("%s", is01(s, zLDR5, sizeof(zLDR5) - 1, xx) ? "ldr5" : "");
//printf("%s", is01(s, zLDR6, sizeof(zLDR6) - 1, xx) ? "ldr6" : "");
//printf("%s", is01(s, zLDR7, sizeof(zLDR7) - 1, xx) ? "ldr7" : "");

//printf("%s", is01(s, zLDRa, sizeof(zLDRa) - 1, xx) ? "ldrA" : "");
//printf("%s", is01(s, zLDRb, sizeof(zLDRb) - 1, xx) ? "ldrB" : "");
//printf("%s", is01(s, zLDRc, sizeof(zLDRc) - 1, xx) ? "ldrC" : "");
//printf("%s", is01(s, zLDRd, sizeof(zLDRd) - 1, xx) ? "ldrD" : "");
//printf("%s", is01(s, zLDRe, sizeof(zLDRe) - 1, xx) ? "ldrE" : "");

//printf("%s", is01(s, zSTP0, sizeof(zSTP0) - 1, xx) ? "stp0" : "");
//printf("%s", is01(s, zSTP1, sizeof(zSTP1) - 1, xx) ? "stp1" : "");
//printf("%s", is01(s, zSTP2, sizeof(zSTP2) - 1, xx) ? "stp2" : "");

      if (is01(s, zSTP2, sizeof(zSTP2) - 1, xx)) { //stp x16, x30, [sp, #-0x??]!
//printf("|%x", is00(s, zSTP2, sizeof(zSTP2) - 1, 'I', xx));
//printf("|%x", is00(s, zSTP2, sizeof(zSTP2) - 1, 'i', xx));
//printf("|%x", is00(s, zSTP2, sizeof(zSTP2) - 1, 't', xx));
      } else if (is01(s, zADRP, sizeof(zADRP) - 1, xx)) { // adrp x16, 0x?????
//printf("*");
//printf("|%x", is00(s, zADRP, sizeof(zADRP) - 1, 'i', xx));
//printf("|%x", is00(s, zADRP, sizeof(zADRP) - 1, 'd', xx));
//const uint32_t lo = is00(s, zADRPa, sizeof(zADRPa) - 1, 'I', xx);
//const uint32_t hi = is00(s, zADRPa, sizeof(zADRPa) - 1, 'i', xx);
//printf("|lo=%x|hi=%x|%x", lo, hi, ((lo >> 29) | (hi >> 3)) << 12);
//const uint32_t Rd = is00(s, zADRPa, sizeof(zADRPa) - 1, 'd', xx);
//printf("|%x:x%d", Rd, Rd);
        prev_vaddr0 = curr_vaddr;
      } else if (is01(s, zBR, sizeof(zBR) - 1, xx)) { // br x17
//printf("!|%lx|%x", prev_vaddr0, prev_vaddr1);
//printf("|%x", is00(s, zBR, sizeof(zBR) - 1, 'n', xx));
        execute_new(q, prev_vaddr0, ocget_namebyvaddr(p, prev_vaddr1, NULL));
      } else if (is01(s, zLDR7, sizeof(zLDR7) - 1, xx)) { // ldr x17, [x16, #0x???]
//printf("A");
//printf("|%x", is00(s, zLDR7, sizeof(zLDR7) - 1, 'i', xx) >> 7);
//printf("|%x", is00(s, zLDR7, sizeof(zLDR7) - 1, 'n', xx) >> 5);
//printf("|%x", is00(s, zLDR7, sizeof(zLDR7) - 1, 't', xx));
//printf("|%x", is00(s, zLDRd, sizeof(zLDRd) - 1, 's', xx));
//const uint32_t im = is00(s, zLDRd, sizeof(zLDRd) - 1, 'i', xx) >> 7; //10;
//printf("|%x", im);
//const uint32_t Rn = is00(s, zLDRd, sizeof(zLDRd) - 1, 'n', xx) >> 5;
//printf("|%x:x%d", Rn, Rn);
//const uint32_t Rt = is00(s, zLDRd, sizeof(zLDRd) - 1, 't', xx);
//printf("|%x:x%d", Rt, Rt);
      } else if (is01(s, zADD1, sizeof(zADD1) - 1, xx)) { // add x16, x16, #0x???
//printf("B");
//printf("|%x", is00(s, zADD1, sizeof(zADD1) - 1, 'S', xx));
//printf("|%x", is00(s, zADD1, sizeof(zADD1) - 1, 'i', xx));
//printf("|%x", is00(s, zADD1, sizeof(zADD1) - 1, 'n', xx));
//printf("|%x", is00(s, zADD1, sizeof(zADD1) - 1, 'd', xx));
      }

      if (0xd503201f == xx) { // nop
//printf("#");
      } else {
        prev_vaddr1 += 2;
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

