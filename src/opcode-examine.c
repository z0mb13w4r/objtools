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

handle_t oecreate(const uint64_t vaddr, unknown_t mnemonic, unknown_t operands) {
  pocexamine_t p = oemalloc();
  if (p) {
    p->vaddr = vaddr;
    p->mc = xmalloc(sizeof(ocmnemonic_t));

    int x = 0;
    if (0 == strncmp(mnemonic, "callq", 5))                x = 5;
    else if (0 == strncmp(mnemonic, "bnd jmpq", 8))        x = 8;
    else if (0 == strncmp(mnemonic, "jmpq", 4))            x = 4;
    else if (0 == strncmp(mnemonic, "jle", 3))             x = 3;
    else if (0 == strncmp(mnemonic, "jmp", 3))             x = 3;
    else if (0 == strncmp(mnemonic, "jne", 3))             x = 3;
    else if (0 == strncmp(mnemonic, "jns", 3))             x = 3;
    else if (0 == strncmp(mnemonic, "je", 2))              x = 2;
    else if (0 == strncmp(mnemonic, "jg", 2))              x = 2;
    else if (0 == strncmp(mnemonic, "js", 2))              x = 2;

    int siz = strlen(mnemonic);

    if (x) {
//printf("++++++++++++++");
      strncpy(p->mc->data, mnemonic, x);
//printf("%s++", p->mc);
      while (x < siz) {
        int c = CAST(uchar_t*, mnemonic)[x];
        if (c != ' ' && c != '\t') break;
        ++x;
      }
//printf("%d+++%s++", x, CAST(puchar_t, mnemonic) + x);
      bool ishex = ishexb(CAST(puchar_t, mnemonic) + x, siz - x);
//printf("%d%s++", siz - x, ishex ? "T" : "F");
      if (ishex) {
        p->op1 = xmalloc(sizeof(ocoperand_t));
        p->op1->uvalue = hexb(CAST(puchar_t, mnemonic) + x, siz - x);
//printf("%x++", p->op1->uvalue);
      }
    }

    const char* s = strchr(mnemonic, '#');
    if (s) {
      strcpy(p->comment, s);
    }
  }

  return p;
}

