#include "arcode.h"

bool_t isAR(const pbuffer_t p) {
  if (issafe(p)) {
    const bool_t isarch = 'a' == getb(p, 2) && 'r' == getb(p, 3) && 'c' == getb(p, 4) && 'h' == getb(p, 5);
    const bool_t isthin = 't' == getb(p, 2) && 'h' == getb(p, 3) && 'i' == getb(p, 4) && 'n' == getb(p, 5);
    return '!' == getb(p, 0) && '<' == getb(p, 1) && (isarch || isthin) && '>' == getb(p, 6) && '\n' == getb(p, 7) ? TRUE : FALSE;
  }

  return FALSE;
}

unknown_t ecget_ahdr(const pbuffer_t p) {
  return getp(p, SARMAG, sizeof(struct ar_hdr));
}

