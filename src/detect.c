#include "dump.h"
#include "detect.h"
#include "pecode.h"
#include "printf.h"
#include "memfind.h"


int detect(const pbuffer_t p, const poptions_t o) {
  if (isPE(p)) {
    dump_summary(p, o);

  } else {
    printf_e("not an PE file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

