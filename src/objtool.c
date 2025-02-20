#include <malloc.h>

#include "buffer.h"
#include "readpe.h"
#include "readelf.h"
#include "objcopy.h"
#include "objdump.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;

  poptions_t o = create(MODE_OPTIONS);
  if (o) {
    if (0 == get_options(o, argc, argv)) {
      pbuffer_t p = open(o->inpname);
      if (p) {
        if (OPT_READELF == o->option) {
          if (isPE(p)) {
            r = readpe(p, o);
          } else if (isELF(p)) {
            r = readelf(p, o);
          }
        } else if (OPT_OBJCOPY == o->option) {
          r = objcopy(p, o);
        } else if (OPT_OBJDUMP == o->option) {
          r = objdump(p, o);
        }
      } else {
        printf("%s: ERROR: '%s': no such file.\n", o->prgname, o->inpname);
      }
      destroy(p);
    }
  }

  destroy(o);

  return r;
}

