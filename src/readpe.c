#include "readpe.h"

int readpe(const pbuffer_t p, const poptions_t o) {
  if (isPE(p)) {

  } else {
    printf("%s: ERROR: not an PE file - it has the wrong magic bytes at the start.\n", o->prgname);
  }

  return 0;
}

