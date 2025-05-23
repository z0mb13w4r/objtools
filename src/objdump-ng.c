#include "opcode.h"
#include "printf.h"
#include "objdump.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = omalloc();
  if (o) {
    r = get_options_objdump(o, argc - 1, argv + 1, argv[0]);
    if (0 == r && o->inpname[0]) {
      occonfig(o->prgname, "x86_64-pc-linux-gnu");

      handle_t p = ocopen(o->inpname);
      if (p) {
        r = objdump(p, o);
      } else {
        printf_e("'%s': no such file.", o->inpname);
      }

      occlose(p);
    }
  }

  xfree(o);
  return r;
}

