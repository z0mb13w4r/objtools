#include "arcode.h"

int isAR(const pbuffer_t p) {
  if (issafe(p)) {
    return '!' == getb(p, 0) && '<' == getb(p, 1) &&
           'a' == getb(p, 2) && 'r' == getb(p, 3) && 'c' == getb(p, 4) && 'h' == getb(p, 5) &&
           '>' == getb(p, 6) && 0x0a == getb(p, 7) ? 1 : 0;
  }

  return 0;
}

