#include "dump.h"
#include "buffer.h"
#include "decode.h"
#include "bstring.h"
#include "options.h"

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = omalloc();
  if (o) {
    r = get_options_convert(o, argc, argv, argv[0]);
    if (0 == r) {
      pbuffer_t p = bopen(o->inpname);
      if (issafe(p) && OPT_CONVERT == o->option) {
        pbstring_t b0 = bstring1(p);
        if (b0) {
          paction_t x = o->actions;
          while (x) {
            dump_actions0(p, x, b0->data, b0->size);
            if (ACT_BASE64D == x->action) {
              b0 = bstring4(b0, base64_decode(b0->data, b0->size));
            } else if (ACT_BASE64E == x->action) {
            }

            x = x->actions;
          }

printf("+++ %d %s\n", __LINE__, b0->data);
          xfree(b0);
        }
      }

      xfree(p);
    }
  }

  xfree(o);
  return r;
}

