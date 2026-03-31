#include <regex.h>

#include "dump.h"
#include "detect.h"
#include "pecode.h"
#include "printf.h"
#include "memfind.h"
#include "objregex.h"

int data_create(const pbuffer_t p, const poptions_t o) {
  pbuffer_t p1 = bopen(o->inpname1);
  if (p1) {
    printf_i("opened: %s", o->inpname1);

    handle_t f = fcalloc(p1->data, p1->size, MEMFIND_NOCHUNKSIZE);
    if (f) {
      pre_t r1 = rmalloc("\\[(.*)\\]");
      pre_t r2 = rmalloc("signature = (([0-9A-F?]{2} ?)+)");
      pre_t r3 = rmalloc("ep_only = (false|true)");

      printf_i("create: %s", o->inpname1);
      while (!fiseof(f)) {
        char *p2 = fgetline(f);
        if (p2 && *p2) {
          int x1 = regex_match(r1, p2);
          if (x1) {
printf("$NAME$ \"%.*s\" is found at position %d to %d (%ld).\n",
            (int)regex_getsize(r1, 1), p2 + regex_getso(r1, 1),
            regex_getso(r1, 1), regex_geteo(r1, 1) - 1, regex_getsize(r1, 1));
          } else {
            int x2 = regex_match(r2, p2);
            if (x2) {
printf("$SIGNATURE$ \"%.*s\" is found at position %d to %d (%ld).\n",
              (int)regex_getsize(r2, 1), p2 + regex_getso(r2, 1),
              regex_getso(r2, 1), regex_geteo(r2, 1) - 1, regex_getsize(r2, 1));
            } else {
              int x3 = regex_match(r3, p2);
              if (x3) {
printf("$FLAG$ \"%.*s\" is found at position %d to %d (%ld).\n",
                (int)regex_getsize(r3, 1), p2 + regex_getso(r3, 1),
                regex_getso(r3, 1), regex_geteo(r3, 1) - 1, regex_getsize(r3, 1));
              }
            }
          }
        }
      }

      rfree(r3);
      rfree(r2);
      rfree(r1);

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

