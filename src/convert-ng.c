#include "dump.h"
#include "buffer.h"
#include "decode.h"
#include "encode.h"
#include "printf.h"
#include "bstring.h"
#include "options.h"
#include "scripts.h"

#include "static/convert-ng.ci"

static int get_options_convert(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    usage0(o, "convert-ng", zCONVERTARGS);
    return -1;
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == strcmp(arg0, "--output")) {
          strncpy(o->outname, arg1, NELEMENTS(o->outname));
        }
      } else {
        o->action |= get_options2(o, zCONVERTARGS, argv[i]);
      }
    } else if ('-' == argv[i][0] && 0 != argv[i][1]) {
      if (0 == strcmp(argv[i], "-O")) {
        strncpy(o->outname, argv[++i], NELEMENTS(o->outname));
      } else {
        o->action |= get_options1(o, zCONVERTARGS, argv[i]);
      }
    } else if (ECODE_ISEVIL(sinsert(o, argv[i]))) {
      strncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, "convert-ng", zCONVERTARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage0(o, "convert-ng", zCONVERTARGS);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int x = ECODE_MALLOC;
  poptions_t o = omalloc();
  if (o) {
    x = get_options_convert(o, argc - 1, argv + 1, argv[0]);
    if (ECODE_ISOK(x) && o->inpname[0]) {
      pbuffer_t p = bopen(o->inpname);
      if (issafe(p)) {
        pbstring_t b0 = bstring1(p);
        if (b0) {
          paction_t x0 = o->actions;
          while (x0) {
            dump_actions0(p, x0, b0->data, b0->size);
            if (ACT_BASE64D == x0->action) {
              b0 = bstring4(b0, base64_decode(b0->data, b0->size));
            } else if (ACT_BASE64E == x0->action) {
              b0 = bstring4(b0, base64_encode(b0->data, b0->size));
            } else if (ACT_DEC8D == x0->action) {
              b0 = bstring4(b0, dec8_decode(b0->data, b0->size));
            } else if (ACT_HEX8E == x0->action) {
              b0 = bstring4(b0, hex8_encode(b0->data, b0->size));
            } else if (ACT_HEX16E == x0->action) {
              b0 = bstring4(b0, hex16_encode(b0->data, b0->size));
            } else if (ACT_HEX32E == x0->action) {
              b0 = bstring4(b0, hex32_encode(b0->data, b0->size));
            }

            x0 = x0->actions;
          }
          if (o->outname[0]) {
            bstrsave(b0, o->outname);
          } else {
            printf_text(b0->data, USE_LT | USE_EOL);
          }
          bstrfree(b0);
        }
      }

      bfree(p);
    }
  }

  ofree(o);
  return x;
}

