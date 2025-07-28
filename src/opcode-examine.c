#include "objutils.h"
#include "opcode-examine.h"

bool_t isocexamine(handle_t p) {
  return ismode(p, MODE_OCEXAMINE);
}

handle_t oemalloc() {
  pocexamine_t p = xmalloc(sizeof(ocexamine_t));
  if (p) {
  }

  return setmode(p, MODE_OCEXAMINE);
}

handle_t oefree(handle_t p) {
  if (isocexamine(p)) {
    pocexamine_t p0 = CAST(pocexamine_t, p);
    free(p0->op1);
    free(p0->op2);
    free(p0->mc);
    free(p);
    return NULL;
  }

  return p;
}

#define OCSTRUCT(x,y) {x, sizeof(x) - 1, y}

#define OCINSTRUCTION_8BIT                         U64MASK(56)
#define OCINSTRUCTION_16BIT                        U64MASK(57)
#define OCINSTRUCTION_32BIT                        U64MASK(58)
#define OCINSTRUCTION_64BIT                        U64MASK(59)
#define OCINSTRUCTION_OPERAND0                     U64MASK(60)
#define OCINSTRUCTION_OPERAND1                     U64MASK(61)
#define OCINSTRUCTION_OPERAND2                     U64MASK(62)

#define OCINSTRUCTION_CALL                         ((1) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_CALLQ                        ((1) | OCINSTRUCTION_OPERAND1 | OCINSTRUCTION_64BIT)
#define OCINSTRUCTION_JMP                          ((2) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_JMPQ                         ((2) | OCINSTRUCTION_OPERAND1 | OCINSTRUCTION_64BIT)
#define OCINSTRUCTION_NOP0                         ((3) | OCINSTRUCTION_OPERAND0)
#define OCINSTRUCTION_NOP1                         ((4) | OCINSTRUCTION_OPERAND1)

#define OCOPERAND_IVALUE                           (1)
#define OCOPERAND_UVALUE                           (2)
#define OCOPERAND_ABSOLUTE                         U64MASK(62)

#define OCREGISTER_GENERAL                         U64MASK(49)
#define OCREGISTER_BASEPTR                         U64MASK(50)
#define OCREGISTER_STACKPTR                        U64MASK(51)
#define OCREGISTER_DSTINDEX                        U64MASK(52)
#define OCREGISTER_SRCINDEX                        U64MASK(53)
#define OCREGISTER_DATA                            U64MASK(54)
#define OCREGISTER_COUNTER                         U64MASK(55)
#define OCREGISTER_BASE                            U64MASK(56)
#define OCREGISTER_ACCUMULATOR                     U64MASK(57)
#define OCREGISTER_8BITLO                          U64MASK(58)
#define OCREGISTER_8BITHI                          U64MASK(59)
#define OCREGISTER_16BIT                           U64MASK(60)
#define OCREGISTER_32BIT                           U64MASK(61)
#define OCREGISTER_64BIT                           U64MASK(62)

#define OCREGISTER_RAX                             ((1) | OCREGISTER_64BIT)
#define OCREGISTER_EAX                             ((2) | OCREGISTER_32BIT)
#define OCREGISTER_AX                              ((3) | OCREGISTER_16BIT)
#define OCREGISTER_AH                              (4)
#define OCREGISTER_AL                              (5)

#define OCREGISTER_RBX                             ((6) | OCREGISTER_64BIT)
#define OCREGISTER_EBX                             ((7) | OCREGISTER_32BIT)
#define OCREGISTER_BX                              ((8) | OCREGISTER_16BIT)
#define OCREGISTER_BH                              (9)
#define OCREGISTER_BL                              (10)

#define OCREGISTER_RCX                             ((11) | OCREGISTER_64BIT)
#define OCREGISTER_ECX                             ((12) | OCREGISTER_32BIT)
#define OCREGISTER_CX                              ((13) | OCREGISTER_16BIT)
#define OCREGISTER_CH                              (14)
#define OCREGISTER_CL                              (15)

#define OCREGISTER_RDX                             ((16) | OCREGISTER_64BIT)
#define OCREGISTER_EDX                             ((17) | OCREGISTER_32BIT)
#define OCREGISTER_DX                              ((18) | OCREGISTER_16BIT)
#define OCREGISTER_DH                              (19)
#define OCREGISTER_DL                              (20)

#define OCREGISTER_R8B                             (21)
#define OCREGISTER_R8D                             ((22) | OCREGISTER_32BIT)
#define OCREGISTER_R8W                             ((23) | OCREGISTER_16BIT)
#define OCREGISTER_R8                              ((24) | OCREGISTER_64BIT)

#define OCREGISTER_R9B                             (25)
#define OCREGISTER_R9D                             ((26) | OCREGISTER_32BIT)
#define OCREGISTER_R9W                             ((27) | OCREGISTER_16BIT)
#define OCREGISTER_R9                              ((28) | OCREGISTER_64BIT)

#define OCREGISTER_R10B                            (29)
#define OCREGISTER_R10D                            ((30) | OCREGISTER_32BIT)
#define OCREGISTER_R10W                            ((31) | OCREGISTER_16BIT)
#define OCREGISTER_R10                             ((32) | OCREGISTER_64BIT)

#define OCREGISTER_R11B                            (33)
#define OCREGISTER_R11D                            ((34) | OCREGISTER_32BIT)
#define OCREGISTER_R11W                            ((35) | OCREGISTER_16BIT)
#define OCREGISTER_R11                             ((36) | OCREGISTER_64BIT)

#define OCREGISTER_R12B                            (37)
#define OCREGISTER_R12D                            ((38) | OCREGISTER_32BIT)
#define OCREGISTER_R12W                            ((39) | OCREGISTER_16BIT)
#define OCREGISTER_R12                             ((40) | OCREGISTER_64BIT)

#define OCREGISTER_R13B                            (41)
#define OCREGISTER_R13D                            ((42) | OCREGISTER_32BIT)
#define OCREGISTER_R13W                            ((43) | OCREGISTER_16BIT)
#define OCREGISTER_R13                             ((44) | OCREGISTER_64BIT)

#define OCREGISTER_R14B                            (45)
#define OCREGISTER_R14D                            ((46) | OCREGISTER_32BIT)
#define OCREGISTER_R14W                            ((47) | OCREGISTER_16BIT)
#define OCREGISTER_R14                             ((48) | OCREGISTER_64BIT)

#define OCREGISTER_R15B                            (49)
#define OCREGISTER_R15D                            ((50) | OCREGISTER_32BIT)
#define OCREGISTER_R15W                            ((51) | OCREGISTER_16BIT)
#define OCREGISTER_R15                             ((52) | OCREGISTER_64BIT)

#define OCREGISTER_RSI                             ((53) | OCREGISTER_64BIT)
#define OCREGISTER_ESI                             ((54) | OCREGISTER_32BIT)
#define OCREGISTER_SI                              ((55) | OCREGISTER_16BIT)

#define OCREGISTER_RDI                             ((56) | OCREGISTER_64BIT)
#define OCREGISTER_EDI                             ((57) | OCREGISTER_32BIT)
#define OCREGISTER_DI                              ((58) | OCREGISTER_16BIT)

#define OCREGISTER_RSP                             ((59) | OCREGISTER_64BIT)
#define OCREGISTER_ESP                             ((60) | OCREGISTER_32BIT)
#define OCREGISTER_SP                              ((61) | OCREGISTER_16BIT)

#define OCREGISTER_RBP                             ((62) | OCREGISTER_64BIT)
#define OCREGISTER_EBP                             ((63) | OCREGISTER_32BIT)
#define OCREGISTER_BP                              ((64) | OCREGISTER_16BIT)

#define OCREGISTER_RIP                             ((65) | OCREGISTER_64BIT)
#define OCREGISTER_EIP                             ((66) | OCREGISTER_32BIT)
#define OCREGISTER_IP                              ((67) | OCREGISTER_16BIT)

typedef struct oestruct_s {
  const char*   mc;
  const size_t  mcsize;
  const imode_t action;
} oestruct_t, *poestruct_t;

static oestruct_t zINSTRUCTIONS[] = {
  OCSTRUCT("callq",     OCINSTRUCTION_CALLQ),

  OCSTRUCT("bnd jmpq",  OCINSTRUCTION_JMP),
  OCSTRUCT("jmpq",      OCINSTRUCTION_JMPQ),
  OCSTRUCT("jle",       OCINSTRUCTION_JMP),
  OCSTRUCT("jmp",       OCINSTRUCTION_JMP),
  OCSTRUCT("jne",       OCINSTRUCTION_JMP),
  OCSTRUCT("jns",       OCINSTRUCTION_JMP),
  OCSTRUCT("je",        OCINSTRUCTION_JMP),
  OCSTRUCT("jg",        OCINSTRUCTION_JMP),
  OCSTRUCT("js",        OCINSTRUCTION_JMP),

  OCSTRUCT("nopl",      OCINSTRUCTION_NOP1),
  OCSTRUCT("nop",       OCINSTRUCTION_NOP0),
  {NULL}
};

static oestruct_t zREGISTERS[] = {
  OCSTRUCT("%rax",     OCREGISTER_RAX),
  OCSTRUCT("%eax",     OCREGISTER_EAX),
  OCSTRUCT("%ax",      OCREGISTER_AX),
  OCSTRUCT("%ah",      OCREGISTER_AH),
  OCSTRUCT("%al",      OCREGISTER_AL),

  OCSTRUCT("%rbx",     OCREGISTER_RBX),
  OCSTRUCT("%ebx",     OCREGISTER_EBX),
  OCSTRUCT("%bx",      OCREGISTER_BX),
  OCSTRUCT("%bh",      OCREGISTER_BH),
  OCSTRUCT("%bl",      OCREGISTER_BL),

  OCSTRUCT("%rcx",     OCREGISTER_RCX),
  OCSTRUCT("%ecx",     OCREGISTER_ECX),
  OCSTRUCT("%cx",      OCREGISTER_CX),
  OCSTRUCT("%ch",      OCREGISTER_CH),
  OCSTRUCT("%cl",      OCREGISTER_CL),

  OCSTRUCT("%rdx",     OCREGISTER_RDX),
  OCSTRUCT("%edx",     OCREGISTER_EDX),
  OCSTRUCT("%dx",      OCREGISTER_DX),
  OCSTRUCT("%dh",      OCREGISTER_DH),
  OCSTRUCT("%dl",      OCREGISTER_DL),

  OCSTRUCT("%r8b",     OCREGISTER_R8B),
  OCSTRUCT("%r8d",     OCREGISTER_R8D),
  OCSTRUCT("%r8w",     OCREGISTER_R8W),
  OCSTRUCT("%r8",      OCREGISTER_R8),

  OCSTRUCT("%r9b",     OCREGISTER_R9B),
  OCSTRUCT("%r9d",     OCREGISTER_R9D),
  OCSTRUCT("%r9w",     OCREGISTER_R9W),
  OCSTRUCT("%r9",      OCREGISTER_R9),

  OCSTRUCT("%r10b",    OCREGISTER_R10B),
  OCSTRUCT("%r10d",    OCREGISTER_R10D),
  OCSTRUCT("%r10w",    OCREGISTER_R10W),
  OCSTRUCT("%r10",     OCREGISTER_R10),

  OCSTRUCT("%r11b",    OCREGISTER_R11B),
  OCSTRUCT("%r11d",    OCREGISTER_R11D),
  OCSTRUCT("%r11w",    OCREGISTER_R11W),
  OCSTRUCT("%r11",     OCREGISTER_R11),

  OCSTRUCT("%r12b",    OCREGISTER_R12B),
  OCSTRUCT("%r12d",    OCREGISTER_R12D),
  OCSTRUCT("%r12w",    OCREGISTER_R12W),
  OCSTRUCT("%r12",     OCREGISTER_R12),

  OCSTRUCT("%r13b",    OCREGISTER_R13B),
  OCSTRUCT("%r13d",    OCREGISTER_R13D),
  OCSTRUCT("%r13w",    OCREGISTER_R13W),
  OCSTRUCT("%r13",     OCREGISTER_R13),

  OCSTRUCT("%r14b",    OCREGISTER_R14B),
  OCSTRUCT("%r14d",    OCREGISTER_R14D),
  OCSTRUCT("%r14w",    OCREGISTER_R14W),
  OCSTRUCT("%r14",     OCREGISTER_R14),

  OCSTRUCT("%r15b",    OCREGISTER_R15B),
  OCSTRUCT("%r15d",    OCREGISTER_R15D),
  OCSTRUCT("%r15w",    OCREGISTER_R15W),
  OCSTRUCT("%r15",     OCREGISTER_R15),

  OCSTRUCT("%rsi",     OCREGISTER_RSI),
  OCSTRUCT("%esi",     OCREGISTER_ESI),
  OCSTRUCT("%si",      OCREGISTER_SI),

  OCSTRUCT("%rdi",     OCREGISTER_RDI),
  OCSTRUCT("%edi",     OCREGISTER_EDI),
  OCSTRUCT("%di",      OCREGISTER_DI),

  OCSTRUCT("%rsp",     OCREGISTER_RSP),
  OCSTRUCT("%esp",     OCREGISTER_ESP),
  OCSTRUCT("%sp",      OCREGISTER_SP),

  OCSTRUCT("%rbp",     OCREGISTER_RBP),
  OCSTRUCT("%ebp",     OCREGISTER_EBP),
  OCSTRUCT("%bp",      OCREGISTER_BP),

  OCSTRUCT("%rip",     OCREGISTER_RIP),
  OCSTRUCT("%eip",     OCREGISTER_EIP),
  OCSTRUCT("%ip",      OCREGISTER_IP),
  {NULL}
};

static poestruct_t oepick(poestruct_t p, unknown_t m, const size_t size) {
  if (m) {
    for (poestruct_t pp = zINSTRUCTIONS; 0 != pp->mc; ++pp) {
      if (0 == strncmp(m, pp->mc, pp->mcsize)) {
        return pp;
      }
    }
  }

  return NULL;
}

static bool oeisskipped(int c) {
  return ' ' == c || '\t' == c ? TRUE : FALSE;
}

static unknown_t oedo_absolute(handle_t p, unknown_t o, unknown_t m) {
  if (isocexamine(p) && o && m) {
    char *m0 = CAST(char*, m);
    pocoperand_t o0 = CAST(pocoperand_t, o);

    if ('*' == *m0) {
      o0->cvalue |= OCOPERAND_ABSOLUTE;
      ++m0;
    }

    return oeskip(m0, strlen(m0));
  }

  return NULL;
}

static unknown_t oedo_value(handle_t p, unknown_t o, unknown_t m) {
  if (isocexamine(p) && o && m) {
    char *m0 = CAST(char*, m);
    pocoperand_t o0 = CAST(pocoperand_t, o);

    size_t m0size = strlen(m0);
    bool ishex = ishexb(m0, m0size);
    if (ishex) {
      o0->uvalue  = hexb(m0, m0size);
      o0->cvalue |= OCOPERAND_UVALUE;
    } else {
//printf("++++%s++++", m0);
    }


    return oeskip(m0, strlen(m0));
  }

  return NULL;
}

unknown_t oeskip(unknown_t p, const size_t size) {
  if (p && 0 != size) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size && *p0; ++i, ++p0) {
      if (!oeisskipped(*p0)) break;
    }

    puchar_t p1 = CAST(puchar_t, p) + size - 1;
    for (size_t i = 0; i < size && *p1; ++i, --p1) {
      if (!oeisskipped(*p1)) break;
      *p1 = 0;
    }

    if ('(' == *p0 && ')' == *p1) {
      ++p0;
      *p1 = 0;
    }

    return p0;
  }

  return NULL;
}

unknown_t oeinsert_comment(handle_t p, unknown_t m) {
  if (isocexamine(p) && m) {
    pocexamine_t p0 = CAST(pocexamine_t, p);
    char* p1 = strchr(m, '#');
    if (p1) {
      strncpy(p0->comment, p1, sizeof(p0->comment));
      *p1 = 0;
    }

    return oeskip(m, strlen(m));
  }

  return NULL;
}

unknown_t oeinsert_mnemonic(handle_t p, unknown_t q, unknown_t m) {
  if (isocexamine(p) && q && m) {
    char *m0 = CAST(char*, m);
    pocexamine_t p0 = CAST(pocexamine_t, p);
    poestruct_t q0 = CAST(poestruct_t, q);

    strncpy(p0->mc->data, q0->mc, q0->mcsize);
    return oeskip(m0 + q0->mcsize, strlen(m0) - q0->mcsize);
  }

  return NULL;
}

unknown_t oeinsert_operand(handle_t p, unknown_t q, unknown_t m) {
  if (isocexamine(p) && q && m) {
    pocoperand_t op = xmalloc(sizeof(ocoperand_t));

    char *m0 = CAST(char *, m);
    m0 = oedo_absolute(p, op, m0);
    m0 = oedo_value(p, op, m0);

    return op;
  }

  return NULL;
}

unknown_t oeinsert_operands(handle_t p, unknown_t q, unknown_t m) {
  if (isocexamine(p) && q && m) {
    pocexamine_t p0 = CAST(pocexamine_t, p);
    poestruct_t q0 = CAST(poestruct_t, q);

    if (MODE_ISSET(q0->action, OCINSTRUCTION_OPERAND1)) {
      p0->op1 = oeinsert_operand(p, q, m);
    }
  }

  return NULL;
}

handle_t oecreate(const uint64_t vaddr, unknown_t mnemonic, unknown_t operands) {
  pocexamine_t p = oemalloc();
  if (p) {
    MALLOCACOPY(char, m0, 160, mnemonic);

    p->vaddr = vaddr;
    p->mc = xmalloc(sizeof(ocmnemonic_t));

    char* m1 = oeinsert_comment(p, m0);
    poestruct_t pi = oepick(zINSTRUCTIONS, m1, strlen(m1));

    if (pi) {
      m1 = oeinsert_mnemonic(p, pi, m1);
      m1 = oeinsert_operands(p, pi, m1);
    }
  }

  return p;
}

