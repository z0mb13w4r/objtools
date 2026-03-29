#include "dump.h"
#include "detect.h"
#include "pecode.h"
#include "printf.h"
#include "memfind.h"

int data_create(const pbuffer_t p, const poptions_t o) {
  pbuffer_t p1 = bopen(o->inpname1);
  if (p1) {
    printf_i("opened: %s", o->inpname1);
  } else {
    printf_e("'%s': no such file.", o->inpname1);
  }

  bfree(p1);

  return 0;
}

int detect(const pbuffer_t p, const poptions_t o) {
  if (isPE(p)) {
    o->action |= OPTPROGRAM_INFO;

    dump_summary(p, o);
    data_create(p, o);
  } else {
    printf_e("not an PE file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

