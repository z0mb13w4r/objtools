#include "pecode.h"

int isPE(const pbuffer_t p) {
  if (issafe(p)) {
    return 'M' == get(p, 0) && 'Z' == get(p, 1) ? 1 : 0;
  }

  return -1;
}

