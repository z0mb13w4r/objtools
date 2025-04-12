#include "buffer.h"
#include "objhash.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = omalloc();
  if (o) {
    r = get_options_objhash(o, argc - 1, argv + 1, argv[0]);
    if (0 == r) {
      pbuffer_t p = bopen(o->inpname);
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

