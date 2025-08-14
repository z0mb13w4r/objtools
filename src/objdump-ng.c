#include "opcode.h"
#include "printf.h"
#include "objdump.h"
#include "options.h"

#include "static/options.ci"
#include "static/objdump-ng.ci"

#define THIS_NAME "objdump-ng"

static int get_options_objdump(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    usage2(o, THIS_NAME, zOBJDUMPARGS, zOBJDUMPARGS0, zOBJDUMPARGS1, zOBJDUMPARGS2, zOBJDUMPARGS3);
    return ECODE_ARGUMENTS;
  }

  strname(o->prgname, name);
  printf_errname(THIS_NAME);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 256);
      MALLOCA(char, arg1, 256);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == strcmp(arg0, zOBJDUMPARGS1)) {
          o->ocdump |= get_options2(o, zDEBUGELFARGS, arg1);
        } else if (0 == strcmp(arg0, zOBJDUMPARGS3)) {
          o->ocdump |= get_options2(o, zDISASSEMBLEARGS, arg1);
        } else if (0 == strcmp(arg0, "--start-address")) {
          o->saddress = atovalue(arg1);
        } else if (0 == strcmp(arg0, "--stop-address")) {
          o->eaddress = atovalue(arg1);
        }
      } else {
        o->action |= get_options2(o, zOBJDUMPARGS, argv[i]);
      }
    } else if ('-' == argv[i][0]) {
      if (0 == strcmp(argv[i], zOBJDUMPARGS0)) {
        imode_t ocdump = get_options1(o, zDEBUGELFARGS, argv[i] + 1);
        o->ocdump |= ocdump ? ocdump : set_options1(o, zDEBUGELFARGS);
      } else if (0 == strcmp(argv[i], zOBJDUMPARGS2)) {
        o->ocdump |= get_options2(o, zDISASSEMBLEARGS, argv[++i]);
      } else {
        o->action |= get_options1(o, zOBJDUMPARGS, argv[i]);
      }
    } else {
      strncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    }
  }

  o->ocdump |= get_ocdump(o, zOBJDUMPSWAP, o->action);

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, THIS_NAME, zOBJDUMPARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage2(o, THIS_NAME, zOBJDUMPARGS, zOBJDUMPARGS0, zOBJDUMPARGS1, zOBJDUMPARGS2, zOBJDUMPARGS3);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int r = ECODE_MALLOC;
  poptions_t o = omalloc();
  if (o) {
    r = get_options_objdump(o, argc - 1, argv + 1, argv[0]);
    if (ECODE_ISOK(r) && 0 != o->inpname[0]) {
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

