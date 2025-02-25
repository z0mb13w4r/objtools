#include "buffer.h"
#include "readpe.h"
#include "readelf.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = create(MODE_OPTIONS);
  if (o) {
    r = get_options_readelf(o, argc, argv, argv[0]);
    if (0 == r) {
      pbuffer_t p = open(o->inpname);
      if (p) {
        if (OPT_READELF == o->option) {
          if (isPE(p)) {
            r = readpe(p, o);
          } else if (isELF(p)) {
            r = readelf(p, o);
          }
        }
      }

      destroy(p);
    }
  }

  destroy(o);
  return r;
}

