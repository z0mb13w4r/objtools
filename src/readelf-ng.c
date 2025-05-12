#include "buffer.h"
#include "readar.h"
#include "readpe.h"
#include "readelf.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = omalloc();
  if (o) {
    r = get_options_readelf(o, argc - 1, argv + 1, argv[0]);
    if (0 == r && o->inpname[0]) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        if (isAR(p)) {
          r = readar(p, o);
        } else if (isPE(p)) {
          r = readpe(p, o);
        } else if (isELF(p)) {
          r = readelf(p, o);
        }
      }

      xfree(p);
    }
  }

  xfree(o);
  return r;
}

