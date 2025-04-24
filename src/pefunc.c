#include "pefunc.h"
#include "objutils.h"

#include "static/ordinals.ci"

const char* get_namebyord(const pbuffer_t p, const char* name, const pick_t x) {
  if (0 == strcasecmp(name, "KERNEL32.dll"))      return strpick(zKERNEL32, x);
  else if (0 == strcasecmp(name, "OLEAUT32.dll")) return strpick(zOLEAUT32, x);
  else if (0 == strcasecmp(name, "WS2_32.dll"))   return strpick(zWS2_32, x);
  else if (0 == strcasecmp(name, "WSOCK32.dll"))  return strpick(zWSOCK32, x);
  return strpickunknown(x);
}

