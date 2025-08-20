#include "opcode-engine.h"

bool_t isocengine(handle_t p) {
  return ismode(p, MODE_OCENGINE);
}

handle_t emalloc() {
  pocengine_t p = xmalloc(sizeof(ocengine_t));
  if (p) {
    p->sizemax = 10000;
    p->groups = xmalloc(sizeof(ocgroups_t) * p->sizemax);
  }

  return setmode(p, MODE_OCENGINE);
}

handle_t efree(handle_t p) {
  if (isocengine(p)) {
    free(p);
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
      pocgroups_t q1 = p1 && p1->groups ? p1->groups + p1->cpos : NULL;

      if (q1 && p1->size < p1->sizemax) {
printf("+++%ld %ld %lx+++", p1->cpos, p1->size + 1, vaddr);
        ++p1->size;
        q1->vaddr = vaddr;
        return q1;
      }
    }
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

