#include "pefunc.h"
#include "objutils.h"

#include "static/ordinals.ci"

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

const char* get_namebyord(const pbuffer_t p, const char* name, const pick_t x) {
  if (0 == strcasecmp(name, "KERNEL32.dll"))      return funcpick(zKERNEL32, x);
  else if (0 == strcasecmp(name, "OLEAUT32.dll")) return funcpick(zOLEAUT32, x);
  else if (0 == strcasecmp(name, "WS2_32.dll"))   return funcpick(zWS2_32, x);
  else if (0 == strcasecmp(name, "WSOCK32.dll"))  return funcpick(zWSOCK32, x);

  return strpickunknown(x);
}

