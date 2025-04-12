#include "buffer.h"
#include "objcopy.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = omalloc();
  if (o) {
    r = get_options_objcopy(o, argc, argv, argv[0]);
    if (0 == r) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        if (OPT_OBJCOPY == o->option) {
          r = objcopy(p, o);
        }
      }

      xfree(p);
    }
  }

  xfree(o);
  return r;
}

