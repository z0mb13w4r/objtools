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
    xfree(p0->op1);
    xfree(p0->op2);
    xfree(p0->mc);
    xfree(p);
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
  } else if (isocexamine(p) && OECODE_OPERAND3 == mode) {
    return CAST(pocexamine_t, p)->op3;
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

size_t oeskiphex(unknown_t p, const size_t size) {
  if (p && 0 != size) {
    puchar_t p0 = CAST(puchar_t, p);

    if (size >= 3 && '$' == p0[0] && '0' == p0[1] && 'x' == p0[2]) {
      return 3;
    } else if (size >= 2 && '0' == p0[0] && 'x' == p0[1]) {
      return 2;
    } else if (size >= 2 && '#' == p0[0] && ' ' == p0[1]) {
      return 2;
    }
  }

  return 0;
}

unknown_t oesplit(handle_t p, unknown_t m, const size_t size, punknown_t o1, punknown_t o2, punknown_t o3) {
  if (isocexamine(p) && m && o1 && o2 && o3) {
    char *m0 = CAST(char*, m);

    size_t i = 0;
    if (i < size) {
      int rbcnt = 0;
      int sbcnt = 0;

      *o1 = m0 + i;
      for ( ; i < size; ++i) {
        char c = m0[i];
        if ('(' == c) ++rbcnt;
        else if (')' == c) --rbcnt;
        else if ('[' == c) ++sbcnt;
        else if (']' == c) --sbcnt;
        else if (',' == c && 0 == rbcnt && 0 == sbcnt) break;
      }

      m0[i++] = 0;
    }

    if (i < size) {
      int rbcnt = 0;
      int sbcnt = 0;

      *o2 = m0 + i;
      for ( ; i < size; ++i) {
        char c = m0[i];
        if ('(' == c) ++rbcnt;
        else if (')' == c) --rbcnt;
        else if ('[' == c) ++sbcnt;
        else if (']' == c) --sbcnt;
        else if (',' == c && 0 == rbcnt && 0 == sbcnt) break;
      }

      m0[i++] = 0;
    }

    if (i < size) {
      *o3 = m0 + i;
    }

    if (*o1) {
//      printf("++%s++", CAST(char*, *o1));
      *o1 = oeskip(*o1, strlen(*o1));
    }
    if (*o2) {
//      printf("%s++", CAST(char*, *o2));
      *o2 = oeskip(*o2, strlen(*o2));
    }
    if (*o3) {
//      printf("%s++", CAST(char*, *o3));
      *o3 = oeskip(*o3, strlen(*o3));
    }

    return *o1;
  }

  return NULL;
}

bool_t oeishexb(unknown_t p, const size_t size) {
  if (p && 0 != size) {
    size_t sz = oeskiphex(p, size);
    return ishexb(CAST(puchar_t, p) + sz, size - sz);
  }

  return TRUE;
}

uint64_t oehexb(unknown_t p, const size_t size) {
  if (p && 0 != size) {
    size_t sz = oeskiphex(p, size);
    return hexb(CAST(puchar_t, p) + sz, size - sz);
  }

  return hexb(p, size);
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
    poestruct_t r0 = oepick(oeREGISTERS, m0, m0size);
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
    poestruct_t s0 = oepick(oeSEGMENTS, m0, m0size);
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
    bool_t ishex = oeishexb(m0, m0size);
    if (ishex) {
      o0->uvalue  = oehexb(m0, m0size);
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
    poestruct_t d0 = oepick(oePREFIXTYPES, m0, m0size);
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
    pocexamine_t p0 = oeget(p, OECODE_THIS);
    pocmnemonic_t p1 = oeget(p, OECODE_MNEMONIC);
    poestruct_t q0 = CAST(poestruct_t, q);

    if (p1) {
      p1->cvalue |= q0->action;
      strncpy(p1->data, q0->mc, q0->mcsize);
      if ('#' == p0->comment[0] && oeishexb(p0->comment, strlen(p0->comment))) {
        p1->uvalue = oehexb(p0->comment, strlen(p0->comment));
      }

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

    if (MODE_ISANY(q0->action, OCINSTRUCTION_OPERAND3)) {
      unknown_t m1 = NULL, m2 = NULL, m3 = NULL;
      oesplit(p, m, strlen(m), &m1, &m2, &m3);
      if (m1) {
        p0->op1 = oeinsert_operand(p, q, m1);
      }
      if (m2) {
        p0->op2 = oeinsert_operand(p, q, m2);
      } else if (MODE_ISNOT(q0->action, OCINSTRUCTION_OPERAND1)) {
        printf_e("Missing operand #2");
      }
      if (m3) {
        p0->op3 = oeinsert_operand(p, q, m3);
      } else if (MODE_ISNOT(q0->action, OCINSTRUCTION_OPERAND2)) {
        printf_e("Missing operand #3");
      }
    } else if (MODE_ISANY(q0->action, OCINSTRUCTION_OPERAND2)) {
      unknown_t m1 = NULL, m2 = NULL, m3 = NULL;
      oesplit(p, m, strlen(m), &m1, &m2, &m3);
      if (m1) {
        p0->op1 = oeinsert_operand(p, q, m1);
      }
      if (m2) {
        p0->op2 = oeinsert_operand(p, q, m2);
      } else if (MODE_ISNOT(q0->action, OCINSTRUCTION_OPERAND1)) {
        printf_e("Missing operand #2");
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
    poestruct_t pi = oepick(oeINSTRUCTIONS, m1, strlen(m1));

    if (pi) {
//printf("++");
      m1 = oeinsert_mnemonic(p0, pi, m1);
      m1 = oeinsert_operands(p0, pi, operands ? operands : m1);
    } else {
      printf_e("The mnemonic is missing from the table oeINSTRUCTIONS");
    }
  }

  return p0;
}

