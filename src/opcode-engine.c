#include "opcode-engine.h"

#define OPENGINE_MAXSIZE (1000)

bool_t isocdebug(handle_t p) {
  return ismode(p, MODE_OCDEBUG);
}

bool_t isocengine(handle_t p) {
  return ismode(p, MODE_OCENGINE);
}

handle_t emalloc() {
  pocengine_t p = xmalloc(sizeof(ocengine_t));
  if (p) {
    p->sizemax = OPENGINE_MAXSIZE;
    p->groups = xmalloc(sizeof(ocgroups_t) * p->sizemax);
  }

  return setmode(p, MODE_OCENGINE);
}

handle_t eresize(handle_t p, const size_t sizemax) {
  if (isocengine(p)) {
    pocengine_t p0 = CAST(pocengine_t, p);
    size_t s1 = sizeof(ocgroups_t) * p0->sizemax;
    size_t s2 = sizeof(ocgroups_t) * sizemax;

    pocgroups_t g0 = xmalloc(s2);
    if (g0) {
      xmemcpy(g0, p0->groups, s1);
      xfree(p0->groups);
      p0->sizemax = sizemax;
      p0->groups = g0;
    }
  }

  return p;
}

handle_t efree(handle_t p) {
  if (isocengine(p)) {
    pocengine_t p0 = CAST(pocengine_t, p);
    pocgroups_t g0 = p0 && p0->groups ? p0->groups : NULL;
    if (g0) {
      for (p0->cpos = 0; p0->cpos < p0->size; ++p0->cpos, ++g0) {
        oefree(g0->examine);
        odfree(g0->debug);
      }
    }
    xfree(p0->groups);
    xfree(p0);
    return NULL;
  }

  return p;
}

handle_t odmalloc() {
  pocdebug_t p = xmalloc(sizeof(ocdebug_t));
  if (p) {
  }

  return setmode(p, MODE_OCDEBUG);
}

handle_t odfree(handle_t p) {
  if (isocdebug(p)) {
    pocdebug_t p0 = CAST(pocdebug_t, p);
    xfree(p0->source);
    xfree(p0->name);
    xfree(p0);
    return NULL;
  }

  return p;
}

handle_t oecreate_engine(handle_t p) {
  if (isopcode(p)) {
    return emalloc();
  }

  return NULL;
}

handle_t oegetbyaddr(handle_t p, const uint64_t vaddr, const imode_t mode) {
  if (isopcode(p)) {
    handle_t p0 = oeseebyaddr(p, vaddr, mode);
    if (NULL == p0 && OPENGINE_GROUP == mode) {
      pocengine_t p1 = ocget(p, OPCODE_ENGINE);
      if (p1->size == p1->sizemax) {
        p1 = eresize(p1, p1->sizemax + OPENGINE_MAXSIZE);
      }

      pocgroups_t g1 = p1 && p1->groups ? p1->groups + p1->cpos : NULL;
      if (g1) {
        if (p1->cpos != p1->size) {
          for (size_t i = p1->size; p1->cpos < i; --i) {
            xmemcpy(p1->groups + i, p1->groups + i - 1, sizeof(ocgroups_t));
          }
        }

        ++p1->size;
        g1->vaddr = vaddr;
        return g1;
      }
    }

    return p0;
  }

  return NULL;
}

handle_t oeseebyaddr(handle_t p, const uint64_t vaddr, const imode_t mode) {
  if (isopcode(p)) {
    pocengine_t p0 = ocget(p, OPCODE_ENGINE);
    pocgroups_t q0 = p0 ? p0->groups : NULL;
    if (q0) {
      for (p0->cpos = 0; p0->cpos < p0->size; ++p0->cpos, ++q0) {
        if (vaddr < q0->vaddr) break;
        else if (vaddr == q0->vaddr) {
          if (OPENGINE_GROUP == mode) {
            return q0;
          } else if (OPENGINE_EXAMINE == mode) {
            return q0->examine;
          }
        }
      }
    }
  }

  return NULL;
}

