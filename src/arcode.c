#include "arcode.h"

int isAR(const pbuffer_t p) {
  if (issafe(p)) {
    return '!' == get(p, 0) && '<' == get(p, 1) &&
           'a' == get(p, 2) && 'r' == get(p, 3) && 'c' == get(p, 4) && 'h' == get(p, 5) &&
           '>' == get(p, 6) && 0x0a == get(p, 7) ? 1 : 0;
  }

  return 0;
}

