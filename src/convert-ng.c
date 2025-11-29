#include "dump.h"
#include "buffer.h"
#include "decode.h"
#include "encode.h"
#include "printf.h"
#include "convert.h"
#include "memfind.h"
#include "options.h"
#include "scripts.h"

#include "static/convert-ng.ci"

#define THIS_NAME "convert-ng"

static int usage(poptions_t o, const char* name, const args_t args[], const int ecode) {
  int n = 0;
  n += usage_name(o, name, args, zDESCRIPTION);
  n += usage_synopsis0(o, name, args);
  n += printf_eol();
  n += usage_description(o, name, args, zCONVERTDESCRIPTION);
  n += usage_options0(o, name, args);
  n += usage_options2(o, name, zSCRIPTCOMMANDS, NULL, NULL);
  n += usage_options2(o, name, zSCRIPTCOMMANDSEX, NULL, NULL);
  n += usage_seealso(o, name, args);
  n += usage_copyright(o, name, args);

  return ecode;
}

static int get_options_convert(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    return usage(o, THIS_NAME, zCONVERTARGS, ECODE_ARGUMENTS);
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == xstrcmp(arg0, "--output")) {
          xstrncpy(o->outname, arg1, NELEMENTS(o->outname));
        } else {
          return odeath(o, THIS_NAME, arg0 + 2);
        }
      } else {
        imode_t action = get_options2(o, zCONVERTARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 2);
        }
        o->action |= action;
      }
    } else if ('-' == argv[i][0] && 0 != argv[i][1]) {
      if (0 == xstrcmp(argv[i], "-O")) {
        xstrncpy(o->outname, argv[++i], NELEMENTS(o->outname));
      } else {
        imode_t action = get_options1(o, zCONVERTARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        o->action |= action;
      }
    } else if (ECODE_ISEVIL(sinsert(o, argv[i], zSCRIPTCOMMANDS, zSCRIPTCOMMANDSEX))) {
      if (0 == o->inpname[0]) {
        xstrncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
      } else {
        return odeath(o, THIS_NAME, argv[i]);
      }
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, THIS_NAME, zCONVERTARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage(o, THIS_NAME, zCONVERTARGS, ECODE_OK);
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
        pfind_t b0 = fcalloc(p->data, p->size, MEMFIND_NOCHUNKSIZE);
        if (b0) {
          bool_t isok = TRUE;
          int32_t step = 0;
          paction_t x0 = o->actions;
          while (x0) {
            if (ACT_BASE32D == x0->action) {
              b0 = fswap(b0, base32_decode(b0->item, b0->size));
            } else if (ACT_BASE32E == x0->action) {
              b0 = fswap(b0, base32_encode(b0->item, b0->size));
            } else if (ACT_BASE58D == x0->action) {
              b0 = fswap(b0, base58_decode(b0->item, b0->size));
            } else if (ACT_BASE58E == x0->action) {
              b0 = fswap(b0, base58_encode(b0->item, b0->size));
            } else if (ACT_BASE64D == x0->action) {
              b0 = fswap(b0, base64_decode(b0->item, b0->size));
            } else if (ACT_BASE64E == x0->action) {
              b0 = fswap(b0, base64_encode(b0->item, b0->size));
            } else if (ACT_BIN8D == x0->action) {
              b0 = fswap(b0, bin_decode(b0->item, b0->size));
            } else if (ACT_BIN8E == x0->action) {
              b0 = fswap(b0, bin8_encode(b0->item, b0->size));
            } else if (ACT_BIN16D == x0->action) {
              b0 = fswap(b0, bin_decode(b0->item, b0->size));
            } else if (ACT_BIN16E == x0->action) {
              b0 = fswap(b0, bin16_encode(b0->item, b0->size));
            } else if (ACT_BIN32D == x0->action) {
              b0 = fswap(b0, bin_decode(b0->item, b0->size));
            } else if (ACT_BIN32E == x0->action) {
              b0 = fswap(b0, bin32_encode(b0->item, b0->size));
            } else if (ACT_DEC8D == x0->action) {
              b0 = fswap(b0, dec8_decode(b0->item, b0->size));
            } else if (ACT_DEC8E == x0->action) {
              b0 = fswap(b0, dec8_encode(b0->item, b0->size));
            } else if (ACT_DEC16D == x0->action) {
              b0 = fswap(b0, dec16_decode(b0->item, b0->size));
            } else if (ACT_DEC16E == x0->action) {
              b0 = fswap(b0, dec16_encode(b0->item, b0->size));
            } else if (ACT_DEC32D == x0->action) {
              b0 = fswap(b0, dec32_decode(b0->item, b0->size));
            } else if (ACT_DEC32E == x0->action) {
              b0 = fswap(b0, dec32_encode(b0->item, b0->size));
            } else if (ACT_HEX8D == x0->action) {
              b0 = fswap(b0, hex_decode(b0->item, b0->size));
            } else if (ACT_HEX8E == x0->action) {
              b0 = fswap(b0, hex8_encode(b0->item, b0->size));
            } else if (ACT_HEX16D == x0->action) {
              b0 = fswap(b0, hex_decode(b0->item, b0->size));
            } else if (ACT_HEX16E == x0->action) {
              b0 = fswap(b0, hex16_encode(b0->item, b0->size));
            } else if (ACT_HEX32D == x0->action) {
              b0 = fswap(b0, hex_decode(b0->item, b0->size));
            } else if (ACT_HEX32E == x0->action) {
              b0 = fswap(b0, hex32_encode(b0->item, b0->size));
            } else if (ACT_ROL8BRUTE == x0->action) {
              dump_rolbrute8(o, b0->item, b0->size, step);
              isok = FALSE;
              break;
            } else if (ACT_ROL16BRUTE == x0->action) {
              dump_rolbrute16(o, b0->item, b0->size, step);
              isok = FALSE;
              break;
            } else if (ACT_ROR8BRUTE == x0->action) {
              dump_rorbrute8(o, b0->item, b0->size, step);
              isok = FALSE;
              break;
            } else if (ACT_ROR16BRUTE == x0->action) {
              dump_rorbrute16(o, b0->item, b0->size, step);
              isok = FALSE;
              break;
            } else if (ACT_SHL8BRUTE == x0->action) {
              dump_shlbrute8(o, b0->item, b0->size, step);
              isok = FALSE;
              break;
            } else if (ACT_SHL16BRUTE == x0->action) {
              dump_shlbrute16(o, b0->item, b0->size, step);
              isok = FALSE;
              break;
            } else if (ACT_SHR8BRUTE == x0->action) {
              dump_shrbrute8(o, b0->item, b0->size, step);
              isok = FALSE;
              break;
            } else if (ACT_SHR16BRUTE == x0->action) {
              dump_shrbrute16(o, b0->item, b0->size, step);
              isok = FALSE;
              break;
            } else if (ACT_XOR8BRUTE == x0->action) {
              dump_xorbrute8(o, b0->item, b0->size, step);
              isok = FALSE;
              break;
            } else if (ACT_XOR16BRUTE == x0->action) {
              dump_xorbrute16(o, b0->item, b0->size, step);
              isok = FALSE;
              break;
            } else if (ACT_INC == x0->action) {
              step =  x0->uvalue;
            } else if (ACT_DEC == x0->action) {
              step = -x0->uvalue;
            } else {
              dump_actions0(p, x0, b0->item, b0->size, step);
            }

            x0 = x0->actions;
          }
          if (o->outname[0]) {
            xset(b0->item, b0->size, o->outname);
          } else if (isok) {
            printf_text(b0->item, USE_LT | USE_EOL);
          }
          ffree(b0);
        }
      } else {
        printf_e("'%s': no such file.", o->inpname);
      }

      bfree(p);
    }

    ofree(o);
  }

  return x;
}

