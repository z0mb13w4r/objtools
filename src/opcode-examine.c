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

#define OCINSTRUCTION(x) x, sizeof(x) - 1

#define OCINSTRUCTION_OPERAND0                     U64MASK(60)
#define OCINSTRUCTION_OPERAND1                     U64MASK(61)
#define OCINSTRUCTION_OPERAND2                     U64MASK(62)
#define OCINSTRUCTION_CALL                         ((1) | OCINSTRUCTION_OPERAND1)
#define OCINSTRUCTION_JMP                          ((2) | OCINSTRUCTION_OPERAND1)

#define OCOPERAND_IVALUE                           (1)
#define OCOPERAND_UVALUE                           (2)

typedef struct ocinstructions_s {
  const char*   mc;
  const size_t  mcsize;
  const imode_t action;
} ocinstructions_t, *pocinstructions_t;

static ocinstructions_t zINSTRUCTIONS[] = {
  {OCINSTRUCTION("callq"),    OCINSTRUCTION_CALL},
  {OCINSTRUCTION("bnd jmpq"), OCINSTRUCTION_JMP},
  {OCINSTRUCTION("jmpq"),     OCINSTRUCTION_JMP},
  {OCINSTRUCTION("jle"),      OCINSTRUCTION_JMP},
  {OCINSTRUCTION("jmp"),      OCINSTRUCTION_JMP},
  {OCINSTRUCTION("jne"),      OCINSTRUCTION_JMP},
  {OCINSTRUCTION("jns"),      OCINSTRUCTION_JMP},
  {OCINSTRUCTION("je"),       OCINSTRUCTION_JMP},
  {OCINSTRUCTION("jg"),       OCINSTRUCTION_JMP},
  {OCINSTRUCTION("js"),       OCINSTRUCTION_JMP},
  {NULL}
};

pocinstructions_t oeget(unknown_t m, const size_t size) {
  if (m) {
    for (pocinstructions_t p = zINSTRUCTIONS; 0 != p->mc; ++p) {
      if (0 == strncmp(m, p->mc, p->mcsize)) {
        return p;
      }
    }
  }

  return NULL;
}

unknown_t oeskip(unknown_t p, const size_t size) {
  if (p && 0 != size) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size && *p0; ++i, ++p0) {
      if (*p0 != ' ' && *p0 != '\t') break;
    }

    return p0;
  }

  return NULL;
}

unknown_t ocget_comment(handle_t p, unknown_t m) {
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

handle_t oecreate(const uint64_t vaddr, unknown_t mnemonic, unknown_t operands) {
  pocexamine_t p = oemalloc();
  if (p) {
    MALLOCACOPY(char, m0, 160, mnemonic);

    p->vaddr = vaddr;
    p->mc = xmalloc(sizeof(ocmnemonic_t));

    char* m1 = ocget_comment(p, m0);
    size_t m1size = strlen(m1);

    pocinstructions_t pi = oeget(m1, m1size);

    if (pi) {
//printf("++++++++++++++");
//printf("%s++", m1);
//printf("%s++", p->comment);
      strncpy(p->mc->data, pi->mc, pi->mcsize);
//printf("%s++", p->mc->data);
      if (MODE_ISSET(pi->action, OCINSTRUCTION_OPERAND1)) {
        char* op = oeskip(m1 + pi->mcsize, m1size - pi->mcsize);
//printf("%s--", op);
        const size_t opsize = strlen(op);
        bool ishex = ishexb(op, opsize);
//printf("%s++", ishex ? "T" : "F");
        if (ishex) {
          p->op1 = xmalloc(sizeof(ocoperand_t));
          p->op1->uvalue = hexb(op, opsize);
          p->op1->cvalue = OCOPERAND_UVALUE;
//printf("%x++", p->op1->uvalue);
        }
      }
    }
  }

  return p;
}

