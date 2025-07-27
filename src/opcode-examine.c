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

#define OCINSTRUCTION(x,y) {x, sizeof(x) - 1, y}

#define OCINSTRUCTION_OPERAND0                     U64MASK(60)
#define OCINSTRUCTION_OPERAND1                     U64MASK(61)
#define OCINSTRUCTION_OPERAND2                     U64MASK(62)
#define OCINSTRUCTION_CALL                         ((1) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_JMP                          ((2) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_NOP0                         ((3) | OCINSTRUCTION_OPERAND0)
#define OCINSTRUCTION_NOP1                         ((4) | OCINSTRUCTION_OPERAND1)

#define OCOPERAND_IVALUE                           (1)
#define OCOPERAND_UVALUE                           (2)

typedef struct ocinstructions_s {
  const char*   mc;
  const size_t  mcsize;
  const imode_t action;
} ocinstructions_t, *pocinstructions_t;

static ocinstructions_t zINSTRUCTIONS[] = {
  OCINSTRUCTION("callq",     OCINSTRUCTION_CALL),

  OCINSTRUCTION("bnd jmpq",  OCINSTRUCTION_JMP),
  OCINSTRUCTION("jmpq",      OCINSTRUCTION_JMP),
  OCINSTRUCTION("jle",       OCINSTRUCTION_JMP),
  OCINSTRUCTION("jmp",       OCINSTRUCTION_JMP),
  OCINSTRUCTION("jne",       OCINSTRUCTION_JMP),
  OCINSTRUCTION("jns",       OCINSTRUCTION_JMP),
  OCINSTRUCTION("je",        OCINSTRUCTION_JMP),
  OCINSTRUCTION("jg",        OCINSTRUCTION_JMP),
  OCINSTRUCTION("js",        OCINSTRUCTION_JMP),

  OCINSTRUCTION("nopl",      OCINSTRUCTION_NOP1),
  OCINSTRUCTION("nop",       OCINSTRUCTION_NOP0),
  {NULL}
};

static pocinstructions_t oeget(unknown_t m, const size_t size) {
  if (m) {
    for (pocinstructions_t p = zINSTRUCTIONS; 0 != p->mc; ++p) {
      if (0 == strncmp(m, p->mc, p->mcsize)) {
        return p;
      }
    }
  }

  return NULL;
}

static bool oeisskipped(int c) {
  return ' ' == c || '\t' == c || '*' == c ? TRUE : FALSE;
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
    pocinstructions_t q0 = CAST(pocinstructions_t, q);

    strncpy(p0->mc->data, q0->mc, q0->mcsize);
    return oeskip(m0 + q0->mcsize, strlen(m0) - q0->mcsize);
  }

  return NULL;
}

unknown_t oeinsert_operand(handle_t p, unknown_t q, unknown_t m) {
  if (isocexamine(p) && q && m) {
    size_t msize = strlen(m);
    bool ishex = ishexb(m, msize);
    if (ishex) {
      pocoperand_t op = xmalloc(sizeof(ocoperand_t));

      op->uvalue = hexb(m, msize);
      op->cvalue = OCOPERAND_UVALUE;

      return op;
    } else {
//printf("++++%s++++", m);
    }
  }

  return NULL;
}

unknown_t oeinsert_operands(handle_t p, unknown_t q, unknown_t m) {
  if (isocexamine(p) && q && m) {
    pocexamine_t p0 = CAST(pocexamine_t, p);
    pocinstructions_t q0 = CAST(pocinstructions_t, q);

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
    pocinstructions_t pi = oeget(m1, strlen(m1));

    if (pi) {
      m1 = oeinsert_mnemonic(p, pi, m1);
      m1 = oeinsert_operands(p, pi, m1);
    }
  }

  return p;
}

