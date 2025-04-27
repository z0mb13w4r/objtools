#include "buffer.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = omalloc();
  if (o) {
    r = get_options_convert(o, argc, argv, argv[0]);
    if (0 == r) {
      pbuffer_t p = bopen(o->inpname);
      if (issafe(p) && OPT_CONVERT == o->option) {
printf("%s\n", p->data);
      }

      xfree(p);
    }
  }

  xfree(o);
  return r;
}

