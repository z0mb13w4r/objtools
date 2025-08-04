#include "objutils.h"
#include "opcode-printf.h"
#include "opcode-examine.h"

#include "static/opcode-examine.ci"

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

unknown_t oeget(handle_t p, const imode_t mode) {
  if (isocexamine(p) && OECODE_THIS == mode) {
    return CAST(pocexamine_t, p);
  } else if (isocexamine(p) && OECODE_MNEMONIC == mode) {
    return CAST(pocexamine_t, p)->mc;
  } else if (isocexamine(p) && OECODE_OPERAND1 == mode) {
    return CAST(pocexamine_t, p)->op1;
  } else if (isocexamine(p) && OECODE_OPERAND2 == mode) {
    return CAST(pocexamine_t, p)->op2;
  }

  return NULL;
}

static poestruct_t oepick(poestruct_t p, unknown_t m, const size_t size) {
  if (m) {
    for (poestruct_t pp = p; 0 != pp->mc; ++pp) {
      if (0 == strncmp(m, pp->mc, pp->mcsize)) {
        return pp;
      }
    }
  }

  return NULL;
}

static bool_t oeisskipped(int c) {
  return ' ' == c || '\t' == c ? TRUE : FALSE;
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

unknown_t oesplit(handle_t p, unknown_t m, const size_t size, punknown_t o1, punknown_t o2, punknown_t o3) {
  if (isocexamine(p) && m && o1 && o2 && o3) {
    char *m0 = CAST(char*, m);

    int rb1cnt = 0;
    int sb1cnt = 0;

    size_t i = 0;
    for ( ; i < size; ++i) {
      char c = m0[i];
      if ('(' == c) ++rb1cnt;
      else if (')' == c) --rb1cnt;
      else if ('[' == c) ++sb1cnt;
      else if (']' == c) --sb1cnt;
      else if (',' == c && 0 == rb1cnt && 0 == sb1cnt) break;
    }

    *o1 = m0;
    m0[i++] = 0;

    if (i < size) {
      *o2 = m0 + i;
      for ( ; i < size; ++i) {
        char c = m0[i];
        if ('(' == c) ++rb1cnt;
        else if (')' == c) --rb1cnt;
        else if ('[' == c) ++sb1cnt;
        else if (']' == c) --sb1cnt;
        else if (',' == c && 0 == rb1cnt && 0 == sb1cnt) break;
      }

      m0[i++] = 0;
    }

    if (i < size) {
      *o3 = m0 + i;
    }

    if (*o1) {
      printf("++%s++", CAST(char*, *o1));
    }
    if (*o2) {
      printf("%s++", CAST(char*, *o2));
    }
    if (*o3) {
      printf("%s++", CAST(char*, *o3));
    }

    return *o1;
  }

  return NULL;
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

static unknown_t oedo_register(handle_t p, unknown_t o, unknown_t m) {
  if (isocexamine(p) && o && m) {
    char *m0 = CAST(char*, m);
    pocoperand_t o0 = CAST(pocoperand_t, o);

    size_t m0size = strlen(m0);
    poestruct_t r0 = oepick(zREGISTERS, m0, m0size);
    if (r0) {
      o0->uvalue  = r0->action;
      o0->cvalue |= OPOPERAND_REGISTER;
//printf("++%s++", m0);
      return oeskip(m0 + r0->mcsize, m0size - r0->mcsize);
    }

    return m0;
  }

  return NULL;
}

static unknown_t oedo_segment(handle_t p, unknown_t o, unknown_t m) {
  if (isocexamine(p) && o && m) {
    char *m0 = CAST(char*, m);
    pocoperand_t o0 = CAST(pocoperand_t, o);

    size_t m0size = strlen(m0);
    poestruct_t s0 = oepick(zSEGMENTS, m0, m0size);
    if (s0) {
      o0->cvalue  = s0->action;
//printf("++%s++", s0->mc);
      return oeskip(m0 + s0->mcsize, m0size - s0->mcsize);
    }

    return m0;
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
//printf("++%s++", m0);
    }


    return oeskip(m0, strlen(m0));
  }

  return NULL;
}

static unknown_t oeinsert_comment(handle_t p, unknown_t m) {
  if (isocexamine(p) && m) {
    pocexamine_t p0 = oeget(p, OECODE_THIS);
    char* p1 = strchr(m, '#');
    if (p1) {
      strncpy(p0->comment, p1, sizeof(p0->comment));
      *p1 = 0;
    }

    return oeskip(m, strlen(m));
  }

  return NULL;
}

static unknown_t oeinsert_prefix(handle_t p, unknown_t m) {
  if (isocexamine(p) && m) {
    char *m0 = CAST(char*, m);
    pocmnemonic_t p1 = oeget(p, OECODE_MNEMONIC);

    size_t m0size = strlen(m0);
    poestruct_t d0 = oepick(zPREFIXTYPES, m0, m0size);
    if (p1 && d0) {
      p1->cvalue |= d0->action;
//printf("++%s++", d0->mc);
      return oeskip(m0 + d0->mcsize, m0size - d0->mcsize);
    }

    return oeskip(m0, m0size);
  }

  return NULL;
}

static unknown_t oeinsert_mnemonic(handle_t p, unknown_t q, unknown_t m) {
  if (isocexamine(p) && q && m) {
    char *m0 = CAST(char*, m);
    pocmnemonic_t p1 = oeget(p, OECODE_MNEMONIC);
    poestruct_t q0 = CAST(poestruct_t, q);

    if (p1) {
      p1->cvalue |= q0->action;
      strncpy(p1->data, q0->mc, q0->mcsize);
      return oeskip(m0 + q0->mcsize, strlen(m0) - q0->mcsize);
    }
  }

  return NULL;
}

static unknown_t oeinsert_operand(handle_t p, unknown_t q, unknown_t m) {
  if (isocexamine(p) && q && m) {
    pocoperand_t o0 = xmalloc(sizeof(ocoperand_t));
    if (o0) {
      char *m0 = CAST(char *, m);
      strncpy(o0->data, m, sizeof(o0->data));

      m0 = oedo_absolute(p, o0, m0);
      m0 = oedo_segment(p, o0, m0);
      m0 = oedo_register(p, o0, m0);
      m0 = oedo_value(p, o0, m0);
    }

    return o0;
  }

  return NULL;
}

static unknown_t oeinsert_operands(handle_t p, unknown_t q, unknown_t m) {
  if (isocexamine(p) && q && m) {
    pocexamine_t p0 = oeget(p, OECODE_THIS);
    poestruct_t q0 = CAST(poestruct_t, q);

    if (MODE_ISANY(q0->action, OCINSTRUCTION_OPERAND2)) {
      unknown_t m1 = NULL, m2 = NULL, m3 = NULL;
      oesplit(p, m, strlen(m), &m1, &m2, &m3);
      if (m1) {
        p0->op1 = oeinsert_operand(p, q, m1);
      }
      if (m2) {
        p0->op2 = oeinsert_operand(p, q, m2);
      }
//printf("+++++++");
    } else if (MODE_ISANY(q0->action, OCINSTRUCTION_OPERAND1)) {
      p0->op1 = oeinsert_operand(p, q, m);
    }
  }

  return NULL;
}

handle_t oecreate(handle_t p, const uint64_t vaddr, unknown_t mnemonic, unknown_t operands) {
  pocexamine_t p0 = oemalloc();
  if (p0) {
    MALLOCACOPY(char, m0, 160, mnemonic);

    p0->vaddr = vaddr;
    p0->mc = xmalloc(sizeof(ocmnemonic_t));

    char* m1 = NULL;
    m1 = oeinsert_comment(p0, m0);
    m1 = oeinsert_prefix(p0, m0);
    poestruct_t pi = oepick(zINSTRUCTIONS, m1, strlen(m1));

    if (pi) {
//printf("++");
      m1 = oeinsert_mnemonic(p0, pi, m1);
      m1 = oeinsert_operands(p0, pi, m1);
    } else {
      printf_e("The mnemonic is missing from the table zINSTRUCTIONS");
    }
  }

  return p0;
}

