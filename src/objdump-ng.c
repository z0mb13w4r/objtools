#include "buffer.h"
#include "objdump.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = create(MODE_OPTIONS);
  if (o) {
    r = get_options_objdump(o, argc, argv, argv[0]);
    if (0 == r) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        if (OPT_OBJDUMP == o->option) {
          r = objdump(p, o);
        }
      }

      destroy(p);
    }
  }

  destroy(o);
  return r;
}

