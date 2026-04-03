#include <regex.h>

#include "dump.h"
#include "detect.h"
#include "pecode.h"
#include "printf.h"
#include "memfind.h"
#include "objregex.h"
#include "signatures.h"

int detect_create(const pbuffer_t p, const poptions_t o) {
  handle_t inp2 = fcalloc(p->data, p->size, MEMFIND_NOBLOCKSIZE);
  handle_t out1 = fcalloc(NULL, p->size, MEMFIND_NOBLOCKSIZE);
  handle_t out2 = fcalloc(NULL, p->size, MEMFIND_NOBLOCKSIZE);

  if (inp2 && out1 && out2) {
    pre_t r1 = rmalloc("\\[(.*)\\]");
    pre_t r2 = rmalloc("signature = (([0-9A-F?]{2} ?)+)");
    pre_t r3 = rmalloc("ep_only = (false|true)");

    fsetchunk(out2, SIGNATURE_MAGIC0, 0);

    while (!fiseof(inp2)) {
      char *p2 = fgetline(inp2);
      if (p2 && *p2) {
        int x1 = regex_match(r1, p2);
        if (x1) {
          if (regex_isvalue(r1, 1)) {
            if (MODE_ISANY(o->action, OPTPROGRAM_VERBOSE)) {
              printf("\n$NAME$ \"%s\" is found at position %d to %d (%ld).\n",
                regex_getvalue(r1, 1), regex_getso(r1, 1), regex_geteo(r1, 1) - 1, regex_getsize(r1, 1));
            }

            fsetchunk(out2, SIGNATURE_NAME, regex_getsize(r1, 1) + 1);
            fsetp(out2, regex_getvalue(r1, 1), regex_getsize(r1, 1) + 1);
          }
        } else {
          int x2 = regex_match(r2, p2);
          if (x2) {
            if (regex_isvalue(r1, 1)) {
              if (MODE_ISANY(o->action, OPTPROGRAM_VERBOSE)) {
                printf("$SIGNATURE$ \"%s\" is found at position %d to %d (%ld).\n",
                regex_getvalue(r2, 1), regex_getso(r2, 1), regex_geteo(r2, 1) - 1, regex_getsize(r2, 1));
              }

              freset(out1);
              const char* c = regex_getvalue(r2, 1);
              for (size_t i = 0; i <= regex_getsize(r2, 1); ++i) {
                const int ch = c[i];
                if (ishex8(ch) || isspecial8(ch)) {
                  fsetu8(out1, ch);
                }
              }
              fsetu8(out1, 0);

              fshrink(out1);
              fsetchunk(out2, SIGNATURE_SIGNATURE, fgetsize(out1));
              fsetp(out2, fget(out1), fgetsize(out1));
            }
          } else {
            int x3 = regex_match(r3, p2);
            if (x3) {
              if (regex_isvalue(r1, 1)) {
                if (MODE_ISANY(o->action, OPTPROGRAM_VERBOSE)) {
                  printf("$FLAG$ \"%s\" is found at position %d to %d (%ld:%s).\n",
                    regex_getvalue(r3, 1), regex_getso(r3, 1), regex_geteo(r3, 1) - 1, regex_getsize(r3, 1),
                    isbool(regex_getvalue(r3, 1), regex_getsize(r3, 1)) ? "y" : "n");
                }

                fsetchunk(out2, SIGNATURE_FLAG, sizeof(uint32_t));
                fsetu32(out2, isbool(regex_getvalue(r3, 1), regex_getsize(r3, 1)) ? SIGNATURE_EP_ONLY : SIGNATURE_NONE);
              }
            }
          }
        }
      }
    }

    rfree(r3);
    rfree(r2);
    rfree(r1);
  }

  fshrink(out2);
  fset(out2, o->outname);

  ffree(inp2);
  ffree(out2);
  ffree(out1);

  return 0;
}

int detect_compare(const pbuffer_t p, const poptions_t o) {
  pbuffer_t s0 = bopen(o->inpname1);
  if (s0) {
    handle_t s1 = fcalloc(s0->data, s0->size, MEMFIND_NOBLOCKSIZE);
    if (s1) {
      size_t  size = 0;
      nmode_t mode = 0;

      mode = fgetchunk(s1, &size);
      if (SIGNATURE_MAGIC0 == mode) {
        fstep(s1, size);

        while (!fiseof(s1)) {
          mode = fgetchunk(s1, &size);
          if (SIGNATURE_NAME == mode) {
            const char* blk = fgetp(s1, size);
printf("$NAME$ = %s\n", blk);
          } else if (SIGNATURE_SIGNATURE == mode) {
            const char* blk = fgetp(s1, size);
printf("$SIGNATURE$ = %s\n", blk);
          } else if (SIGNATURE_FLAG == mode) {
            const uint32_t val = fgetu32(s1);
printf("$FLAG$ = %x\n", val);
          } else {
            printf_e("'%s': bad magic %08x.", o->inpname1, mode);
            break;
          }

//          fstep(s1, size);
        }
      }

      ffree(s1);
    }

    bfree(s0);
  } else {
    printf_e("'%s': no such file.", o->inpname1);
  }

  return 0;
}

int detect(const pbuffer_t p, const poptions_t o) {
  if (isPE(p)) {
    o->action |= OPTPROGRAM_INFO;

    dump_summary(p, o);
    detect_compare(p, o);

  } else {
    printf_e("not an PE file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

