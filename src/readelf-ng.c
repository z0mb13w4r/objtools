#include "buffer.h"
#include "readelf.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = create(MODE_OPTIONS);
  if (o) {
    if (0 == get_options_readelf(o, argc, argv)) {
      pbuffer_t p = open(o->inpname);
      if (p) {
        if (OPT_READELF == o->option) {
          r = readelf(p, o);
        }
      }

      destroy(p);
    }
  }

  destroy(o);
  return r;
}

