#include "buffer.h"
#include "printf.h"
#include "readar.h"
#include "readpe.h"
#include "readelf.h"
#include "objcopy.h"
#include "objhash.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;

  poptions_t o = omalloc();
  if (o) {
    r = get_options(o, argc, argv);
    if (0 == r) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        if (OPT_READELF == o->option) {
          if (isAR(p)) {
            r = readar(p, o);
          } else if (isPE(p)) {
            r = readpe(p, o);
          } else if (isELF(p)) {
            r = readelf(p, o);
          }
        } else if (OPT_OBJCOPY == o->option) {
          r = objcopy(p, o);
        } else if (OPT_OBJHASH == o->option) {
          r = objhash(p, o);
        }
      } else {
        printf_e("'%s': no such file.", o->inpname);
      }

      xfree(p);
    }
  }

  xfree(o);
  return r;
}

