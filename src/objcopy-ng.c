#include "buffer.h"
#include "objcopy.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = create(MODE_OPTIONS);
  if (o) {
    if (0 == get_options_objcopy(o, argc, argv, argv[0])) {
      pbuffer_t p = open(o->inpname);
      if (p) {
        if (OPT_OBJCOPY == o->option) {
          r = objcopy(p, o);
        }
      }

      destroy(p);
    }
  }

  destroy(o);
  return r;
}

