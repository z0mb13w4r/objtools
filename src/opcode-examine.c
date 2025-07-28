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

#define OCINSTRUCTION_OPERAND0                     U64MASK(60)
#define OCINSTRUCTION_OPERAND1                     U64MASK(61)
#define OCINSTRUCTION_OPERAND2                     U64MASK(62)
#define OCINSTRUCTION_CALL                         ((1) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_JMP                          ((2) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_NOP0                         ((3) | OCINSTRUCTION_OPERAND0)
#define OCINSTRUCTION_NOP1                         ((4) | OCINSTRUCTION_OPERAND1)

#define OCOPERAND_IVALUE                           (1)
#define OCOPERAND_UVALUE                           (2)
#define OCOPERAND_ABSOLUTE                         U64MASK(62)

typedef struct oestruct_s {
  const char*   mc;
  const size_t  mcsize;
  const imode_t action;
} oestruct_t, *poestruct_t;

static oestruct_t zINSTRUCTIONS[] = {
  OCSTRUCT("callq",     OCINSTRUCTION_CALL),

  OCSTRUCT("bnd jmpq",  OCINSTRUCTION_JMP),
  OCSTRUCT("jmpq",      OCINSTRUCTION_JMP),
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

