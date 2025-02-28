#include "arcode.h"
#include "printf.h"
#include "readar.h"
#include "objutils.h"

int readar(const pbuffer_t p, const poptions_t o) {
  if (isAR(p)) {
    printf_w("archive files not supported at this time.");
  } else {
    printf_e("not an archive file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

