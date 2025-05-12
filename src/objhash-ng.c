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
        r = objhash(p, o);
      }

      xfree(p);
    }
  }

  xfree(o);
  return r;
}

