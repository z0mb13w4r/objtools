#include "dump.h"
#include "detect.h"
#include "pecode.h"
#include "printf.h"
#include "memfind.h"

int data_create(const pbuffer_t p, const poptions_t o) {
  pbuffer_t p1 = bopen(o->inpname1);
  if (p1) {
    printf_i("opened: %s", o->inpname1);

    handle_t f = fcalloc(p1->data, p1->size, MEMFIND_NOCHUNKSIZE);
    if (f) {
      printf_i("create: %s", o->inpname1);
      while (!fiseof(f)) {
        char *p2 = fgetline(f);
        if (p2 && *p2) {
          if (';' == *p2) {
printf("$COMMENT$");
          } else if ('[' == *p2) {
printf("$NAME");
          } else if (0 == xstrncmp(p2, "signature = ", 12)) {
printf("$SIGNATURE$");
          } else if (0 == xstrncmp(p2, "ep_only = ", 10)) {
printf("$FLAG$");
          }
printf("%s\n", p2);
        }
      }

      ffree(f);
    }

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

