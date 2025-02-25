#include "buffer.h"
#include "objhash.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = create(MODE_OPTIONS);
  if (o) {
    r = get_options_objhash(o, argc, argv, argv[0]);
    if (0 == r) {
      pbuffer_t p = open(o->inpname);
      if (p) {
        if (OPT_OBJHASH == o->option) {
          r = objhash(p, o);
        }
      }

      destroy(p);
    }
  }

  destroy(o);
  return r;
}

