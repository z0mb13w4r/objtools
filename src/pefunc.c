#include "pefunc.h"
#include "objutils.h"

#include "static/pefuncs.ci"

const char* funcpick(const ppefunc_t p, const pick_t x) {
  const char* s = funcpicknull(p, x);
  if (NULL == s) {
    return strpickunknown(x);
  }

  return s;
}

const char* funcpicknull(const ppefunc_t p, const pick_t x) {
  if (p) {
    for (ppefunc_t pp = p; 0 != pp->text; ++pp) {
      if (pp->ords == x) {
        return pp->text;
      }
    }
  }

  return NULL;
}

ppefunc_t funcchoice(const ppefunc_t p, const char* name) {
  if (p) {
    for (ppefunc_t pp = p; 0 != pp->text; ++pp) {
      if (0 == strcasecmp(pp->text, name)) {
        return pp;
      }
    }
  }

  return NULL;
}

const char* get_ORDDLL(const pbuffer_t p, const char* name, const pick_t x) {
  if (name) {
    for (ppeobject_t pp = zPEOBJECTS; 0 != pp->name; ++pp) {
      if (0 == strcasecmp(name, pp->name))      return funcpick(pp->func, x);
    }
  }

  return strpickunknown(x);
}

imode_t get_MODEFUNC(const pbuffer_t p, const char* name) {
  if (name) {
    for (ppeobject_t pp = zPEOBJECTS; 0 != pp->name; ++pp) {
      ppefunc_t p0 = funcchoice(pp->func, name);
      if (p0) return p0->mode;
    }
  }

  return PEFUNC_UNKNOWN;
}

