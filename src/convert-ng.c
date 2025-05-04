#include "dump.h"
#include "buffer.h"
#include "decode.h"
#include "encode.h"
#include "printf.h"
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
              b0 = bstring4(b0, base64_encode(b0->data, b0->size));
            } else if (ACT_HEX8E == x->action) {
              b0 = bstring4(b0, hex8_encode(b0->data, b0->size));
            } else if (ACT_HEX16E == x->action) {
              b0 = bstring4(b0, hex16_encode(b0->data, b0->size));
            } else if (ACT_HEX32E == x->action) {
              b0 = bstring4(b0, hex32_encode(b0->data, b0->size));
            }

            x = x->actions;
          }

          printf_text(b0->data, USE_LT | USE_EOL);
          xfree(b0);
        }
      }

      xfree(p);
    }
  }

  xfree(o);
  return r;
}

