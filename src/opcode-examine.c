#include "objutils.h"
#include "opcode-printf.h"
#include "opcode-examine.h"

#include "static/opcode-examine.ci"

#define USE_STRLEN             (-1)

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
  } else if (isocexamine(p) && OECODE_OPERAND4 == mode) {
    return CAST(pocexamine_t, p)->op4;
  }

  return NULL;
}

static bool_t oeisok0(const uchar_t c) {
  return 0 == c || ':' == c;
}

static bool_t oeisok1(const uchar_t c) {
  return oeisok0(c) || ' ' == c || ',' == c || '(' == c;
}

static poestruct_t oepick(poestruct_t p, unknown_t m, const size_t size) {
  if (m && USE_STRLEN == size) {
    return oepick(p, m, xstrlen(m));
  } else if (m && size) {
    for (poestruct_t pp = p; 0 != pp->mc; ++pp) {
      if (0 == xstrncmp(m, pp->mc, pp->mcsize) && (oeisok0(pp->mc[pp->mcsize - 1]) || oeisok1(CAST(puchar_t, m)[pp->mcsize]))) {
        return pp;
      }
    }
  }

  return NULL;
}

static poestruct_t oepick_REG(unknown_t m, const size_t size) {
  if (m && USE_STRLEN == size) {
    return oepick_REG(m, xstrlen(m));
  } else if (m && size) {
    return oepick(oeREGISTERS, m, size);
  }

  return NULL;
}

static poestruct_t oepick_SEG(unknown_t m, const size_t size) {
  if (m && USE_STRLEN == size) {
    return oepick_SEG(m, xstrlen(m));
  } else if (m && size) {
    poestruct_t p0 = oepick(oeSEGMENTS, m, size);
    return p0 ? p0 : oepick(oeSIZEPTRS, m, size);
  }

  return NULL;
}

static bool_t oeisskipped(int c) {
  return ' ' == c || '\t' == c || '%' == c ? TRUE : FALSE;
}

static bool_t oeisstripped(int c0, int c1) {
  return ('(' == c0 && ')' == c1) || ('[' == c0 && ']' == c1);
}

unknown_t oeskip(unknown_t p, const size_t size) {
  if (p && USE_STRLEN == size) {
    return oeskip(p, xstrlen(p));
  } else if (p && 0 != size) {
    puchar_t p0 = CAST(puchar_t, p);
    for (size_t i = 0; i < size && *p0; ++i, ++p0) {
      if (!oeisskipped(*p0)) break;
    }

    puchar_t p1 = CAST(puchar_t, p) + size - 1;
    for (size_t i = 0; i < size && *p1; ++i, --p1) {
      if (!oeisskipped(*p1)) break;
      *p1 = 0;
    }

    if (oeisstripped(*p0, *p1)) {
      ++p0;
      *p1 = 0;
    }

    return p0;
  }

  return NULL;
}

size_t oeskipdec(unknown_t p, const size_t size) {
  if (p && 0 != size) {
    puchar_t p0 = CAST(puchar_t, p);

    if (size >= 3 && '$' == p0[0] && '0' == p0[1] && 'x' == p0[2]) {
      return 0;
    } else if (size >= 2 && '-' == p0[0] && ' ' == p0[1]) {
      return 2;
    } else if (size >= 1 && '-' == p0[0]) {
      return 1;
    } else if (size >= 2 && '+' == p0[0] && ' ' == p0[1]) {
      return 2;
    } else if (size >= 1 && '+' == p0[0]) {
      return 1;
    } else if (size >= 1 && '$' == p0[0]) {
      return 1;
    }
  }

  return 0;
}

size_t oeskiphex(unknown_t p, const size_t size) {
  if (p && 0 != size) {
    puchar_t p0 = CAST(puchar_t, p);

    if (size >= 3 && '$' == p0[0] && '0' == p0[1] && 'x' == p0[2]) {
      return 3;
    } else if (size >= 4 && '-' == p0[0] && ' ' == p0[1] && '0' == p0[2] && 'x' == p0[3]) {
      return 4;
    } else if (size >= 3 && '-' == p0[0] && '0' == p0[1] && 'x' == p0[2]) {
      return 3;
    } else if (size >= 4 && '+' == p0[0] && ' ' == p0[1] && '0' == p0[2] && 'x' == p0[3]) {
      return 4;
    } else if (size >= 3 && '+' == p0[0] && '0' == p0[1] && 'x' == p0[2]) {
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
  if (m && USE_STRLEN == size) {
    return oesplit(p, m, xstrlen(m), o1, o2, o3);
  } else if (isocexamine(p) && m && o1 && o2 && o3) {
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
      *o1 = oeskip(*o1, USE_STRLEN);
    }
    if (*o2) {
//      printf("%s++", CAST(char*, *o2));
      *o2 = oeskip(*o2, USE_STRLEN);
    }
    if (*o3) {
//      printf("%s++", CAST(char*, *o3));
      *o3 = oeskip(*o3, USE_STRLEN);
    }

    return *o1;
  }

  return NULL;
}

bool_t oeisdecb(unknown_t p, const size_t size) {
  if (p && USE_STRLEN == size) {
    return oeisdecb(p, xstrlen(p));
  } else if (p && size) {
    size_t sz = oeskipdec(p, size);
    return isdecb(CAST(puchar_t, p) + sz, size - sz);
  }

  return FALSE;
}

bool_t oeishexb(unknown_t p, const size_t size) {
  if (p && USE_STRLEN == size) {
    return oeishexb(p, xstrlen(p));
  } else if (p && size) {
    size_t sz = oeskiphex(p, size);
    return ishexb(CAST(puchar_t, p) + sz, size - sz);
  }

  return FALSE;
}

uint64_t oedecb(unknown_t p, const size_t size) {
  if (p && USE_STRLEN == size) {
    return oedecb(p, xstrlen(p));
  } else if (p && size) {
    size_t sz = oeskipdec(p, size);
    puchar_t p0 = CAST(puchar_t, p);
    return '-' == p0[0] ? -decb(p0 + sz, size - sz) : decb(p0 + sz, size - sz);
  }

  return decb(p, size);
}

uint64_t oehexb(unknown_t p, const size_t size) {
  if (p && USE_STRLEN == size) {
    return oehexb(p, xstrlen(p));
  } else if (p && size) {
    size_t sz = oeskiphex(p, size);
    puchar_t p0 = CAST(puchar_t, p);
    return '-' == p0[0] ? -hexb(p0 + sz, size - sz) : hexb(p0 + sz, size - sz);
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

    return oeskip(m0, USE_STRLEN);
  }

  return NULL;
}

static unknown_t oedo_register(handle_t p, unknown_t o, unknown_t m) {
  if (isocexamine(p) && o && m) {
    char *m0 = CAST(char*, m);
    pocoperand_t o0 = CAST(pocoperand_t, o);

    size_t m0size = xstrlen(m0);
    poestruct_t r0 = oepick_REG(m0, m0size);
    if (r0) {
      o0->uvalue0 = r0->action;
      o0->cvalue |= OPOPERAND_REGISTER0;
//printf("++%s:%s:%lx++", m0, r0->mc, r0->action);
      m0 = oeskip(m0 + r0->mcsize, m0size - r0->mcsize);
      if (m0 && (',' == m0[0] || '+' == m0[0])) {
        m0 = oeskip(m0 + 1, USE_STRLEN);
      }

      if (oeishexb(m0, USE_STRLEN)) {
        o0->uvalue1 = oehexb(m0, USE_STRLEN);
        o0->cvalue |= OCOPERAND_UVALUE1;
        m0 = NULL;
      } else if (oeisdecb(m0, USE_STRLEN)) {
        o0->ivalue1 = oedecb(m0, USE_STRLEN);
        o0->cvalue |= OCOPERAND_IVALUE1;
        m0 = NULL;
      } else {
        poestruct_t r1 = oepick_REG(m0, USE_STRLEN);
        if (r1) {
          o0->uvalue1 = r1->action;
          o0->cvalue |= OPOPERAND_REGISTER1;
//printf("++%s:%s:%lx++", m0, r1->mc, r1->action);
          m0 = oeskip(m0 + r1->mcsize, xstrlen(m0) - r1->mcsize);
        }
      }

      if (m0 && (',' == m0[0] || '+' == m0[0])) {
        m0 = oeskip(m0 + 1, USE_STRLEN);
      }

      if (oeishexb(m0, USE_STRLEN)) {
        o0->uvalue2 = oehexb(m0, USE_STRLEN);
        o0->cvalue |= OCOPERAND_UVALUE2;
        m0 = NULL;
      } else if (oeisdecb(m0, USE_STRLEN)) {
        o0->ivalue2 = oedecb(m0, USE_STRLEN);
        o0->cvalue |= OCOPERAND_IVALUE2;
        m0 = NULL;
      }

#ifdef OPCODE_EXAMINE_DEBUGX
      if (m0) {
        printf_e("The operand has not been processed '%s'", m0);
      }
#endif
      return NULL;
    }

    return m0;
  }

  return NULL;
}

static unknown_t oedo_segment(handle_t p, unknown_t o, unknown_t m) {
  if (isocexamine(p) && o && m) {
    char *m0 = CAST(char*, m);
    pocoperand_t o0 = CAST(pocoperand_t, o);
    size_t m0size = xstrlen(m0);
    poestruct_t s0 = oepick_SEG(m0, m0size);
    if (s0) {
      o0->cvalue |= s0->action;
//printf("++%s++", s0->mc);
      m0 = oeskip(m0 + s0->mcsize, m0size - s0->mcsize);
      if (m0) {
        size_t m1size = xstrlen(m0);
        poestruct_t s1 = oepick_SEG(m0, m1size);
        if (s1) {
          o0->cvalue |= s1->action;
//printf("++%s++", s1->mc);
          m0 = oeskip(m0 + s1->mcsize, m1size - s1->mcsize);
        }
      }
    }

    return m0;
  }

  return NULL;
}

static unknown_t oedo_value(handle_t p, unknown_t o, unknown_t m) {
  if (isocexamine(p) && o && m) {
    char *m0 = CAST(char*, m);
    pocoperand_t o0 = CAST(pocoperand_t, o);

    size_t m0size = xstrichr(m0, '(');
    m0size = -1 == m0size ? xstrlen(m0) : m0size;

    bool_t ishex = oeishexb(m0, m0size);
    bool_t isnum = oeisdecb(m0, m0size);
    if (ishex) {
      o0->uvalue0 = oehexb(m0, m0size);
      o0->cvalue |= OCOPERAND_UVALUE0;
    } else if (isnum) {
      o0->ivalue0 = oedecb(m0, m0size);
      o0->cvalue |= OCOPERAND_IVALUE0;
    } else {
//printf("++%s++", m0);
    }

    m0 = oeskip(m0 + m0size, USE_STRLEN);
    if (m0) {
//printf("++%s++\n", m0);
      m0size = xstrlen(m0);

      unknown_t m1 = NULL, m2 = NULL, m3 = NULL;
      oesplit(p, m0, USE_STRLEN, &m1, &m2, &m3);
//printf("++%s+%s+%s++", CAST(char*, m1), CAST(char*, m2), CAST(char*, m3));

      poestruct_t r1 = oepick_REG(m1, USE_STRLEN);
      if (r1) {
        o0->uvalue1 = r1->action;
        o0->cvalue |= OPOPERAND_REGISTER1;
//printf("++%s++", m0);
        m1 = oeskip(m1 + r1->mcsize, xstrlen(m1) - r1->mcsize);
      } else if (oeishexb(m1, USE_STRLEN)) {
        o0->uvalue1 = oehexb(m1, USE_STRLEN);
        o0->cvalue |= OCOPERAND_UVALUE1;
        m1 = NULL;
      }

      poestruct_t r2 = oepick_REG(m2, USE_STRLEN);
      if (r2) {
        o0->uvalue2 = r2->action;
        o0->cvalue |= OPOPERAND_REGISTER2;
//printf("++%s++", m2);
        m2 = oeskip(m2 + r2->mcsize, xstrlen(m2) - r2->mcsize);
      } else if (oeishexb(m2, USE_STRLEN)) {
        o0->uvalue2 = oehexb(m2, USE_STRLEN);
        o0->cvalue |= OCOPERAND_UVALUE2;
        m2 = NULL;
      }

      poestruct_t r3 = oepick_REG(m3, USE_STRLEN);
      if (r3) {
        o0->uvalue3 = r3->action;
        o0->cvalue |= OPOPERAND_REGISTER3;
//printf("++%s++", m3);
        m3 = oeskip(m3 + r3->mcsize, xstrlen(m3) - r3->mcsize);
      } else if (oeishexb(m3, USE_STRLEN)) {
        o0->uvalue3 = oehexb(m3, USE_STRLEN);
        o0->cvalue |= OCOPERAND_UVALUE3;
        m3 = NULL;
      }
#ifdef OPCODE_EXAMINE_DEBUGX
      if (m1 || m2 || m3) {
        printf_e("The operand has not been processed '%s:%s:%s'", m1, m2, m3);
      }
#endif
    }
  }

  return NULL;
}

static unknown_t oeinsert_comment(handle_t p, unknown_t m) {
  if (isocexamine(p) && m) {
    pocexamine_t p0 = oeget(p, OECODE_THIS);
    char* p1 = strchr(m, '#');
    if (p1) {
      xstrncpy(p0->comment, p1, sizeof(p0->comment));
      *p1 = 0;
    }

    return oeskip(m, USE_STRLEN);
  }

  return NULL;
}

static unknown_t oeinsert_prefix(handle_t p, unknown_t m) {
  if (isocexamine(p) && m) {
    char *m0 = CAST(char*, m);
    pocmnemonic_t p1 = oeget(p, OECODE_MNEMONIC);

    size_t m0size = xstrlen(m0);
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
      xstrncpy(p1->data, q0->mc, q0->mcsize);
      if ('#' == p0->comment[0] && oeishexb(p0->comment, USE_STRLEN)) {
        p1->uvalue = oehexb(p0->comment, USE_STRLEN);
      }

      return oeskip(m0 + q0->mcsize, xstrlen(m0) - q0->mcsize);
    }
  }

  return NULL;
}

static unknown_t oeinsert_operand(handle_t p, unknown_t q, unknown_t m) {
  if (isocexamine(p) && q && m) {
    pocoperand_t o0 = xmalloc(sizeof(ocoperand_t));
    if (o0) {
      char *m0 = CAST(char *, m);
      xstrncpy(o0->data, m0, sizeof(o0->data));

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
      oesplit(p, m, USE_STRLEN, &m1, &m2, &m3);
      if (m1) {
        p0->op1 = oeinsert_operand(p, q, m1);
      } else if (MODE_ISNOT(q0->action, OCINSTRUCTION_OPERAND0)) {
#ifdef OPCODE_EXAMINE_DEBUGX
        printf_e("Missing operand #1");
#endif
      } else {
        p0->mc->cvalue &= ~OCINSTRUCTION_OPERAND1;
      }
      if (m2) {
        p0->op2 = oeinsert_operand(p, q, m2);
      } else if (MODE_ISNOT(q0->action, OCINSTRUCTION_OPERAND1)) {
#ifdef OPCODE_EXAMINE_DEBUGX
        printf_e("Missing operand #2");
#endif
      } else {
        p0->mc->cvalue &= ~OCINSTRUCTION_OPERAND2;
      }
      if (m3) {
        p0->op3 = oeinsert_operand(p, q, m3);
      } else if (MODE_ISNOT(q0->action, OCINSTRUCTION_OPERAND2)) {
#ifdef OPCODE_EXAMINE_DEBUGX
        printf_e("Missing operand #3");
#endif
      } else {
        p0->mc->cvalue &= ~OCINSTRUCTION_OPERAND3;
      }
    } else if (MODE_ISANY(q0->action, OCINSTRUCTION_OPERAND2)) {
      unknown_t m1 = NULL, m2 = NULL, m3 = NULL;
      oesplit(p, m, USE_STRLEN, &m1, &m2, &m3);
      if (m1) {
        p0->op1 = oeinsert_operand(p, q, m1);
      }
      if (m2) {
        p0->op2 = oeinsert_operand(p, q, m2);
      } else if (MODE_ISNOT(q0->action, OCINSTRUCTION_OPERAND1)) {
#ifdef OPCODE_EXAMINE_DEBUGX
        printf_e("Missing operand #2");
#endif
      } else {
        p0->mc->cvalue &= ~OCINSTRUCTION_OPERAND2;
      }
//printf("+++++++");
    } else if (MODE_ISANY(q0->action, OCINSTRUCTION_OPERAND1)) {
      unknown_t m1 = NULL, m2 = NULL, m3 = NULL;
      oesplit(p, m, USE_STRLEN, &m1, &m2, &m3);
      if (m1) {
        p0->op1 = oeinsert_operand(p, q, m1);
      } else if (MODE_ISNOT(q0->action, OCINSTRUCTION_OPERAND0)) {
#ifdef OPCODE_EXAMINE_DEBUGX
        printf_e("Missing operand #1");
#endif
      } else {
        p0->mc->cvalue &= ~OCINSTRUCTION_OPERAND1;
      }
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
    poestruct_t pi = oepick(oeINSTRUCTIONS, m1, USE_STRLEN);

    if (pi) {
//printf("++");
      m1 = oeinsert_mnemonic(p0, pi, m1);
      m1 = oeinsert_operands(p0, pi, operands ? operands : m1);
    } else {
#ifdef OPCODE_EXAMINE_DEBUGX
      printf_e("The mnemonic is missing from the table oeINSTRUCTIONS");
#endif
    }
  }

  return p0;
}

