#include "readpe.h"
#include "printf.h"

int readpe(const pbuffer_t p, const poptions_t o) {
  if (isPE(p)) {

  } else {
    printf_e("not an PE file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

