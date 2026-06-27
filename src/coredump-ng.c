#include "buffer.h"
#include "printf.h"
#include "scripts.h"
#include "options.h"
#include "coredump.h"

#include "static/coredump-ng.ci"

#define THIS_NAME "coredump-ng"

static int usage(poptions_t o, const char* name, const args_t args[], const int ecode) {
  int n = 0;
  n += usage_name(o, name, args, zDESCRIPTIONELF);
  n += usage_synopsis0(o, name, args);
//  n += usage_synopsis1(o, name, zDEBUGELFARGS, zREADELFARGS0, zREADELFARGS1);
//  n += usage_synopsis2(o, name, zDISASSEMBLEARGS, zREADELFARGS2, zREADELFARGS3);
  n += usage_objname(o, name, "elffile");
  n += printf_eol();
  n += usage_description(o, name, args, zELFDESCRIPTION);
  n += usage_options0(o, name, args);
//  n += usage_options1(o, name, zDEBUGELFARGS, zREADELFARGS0, zREADELFARGS1);
//  n += usage_options2(o, name, zDISASSEMBLEARGS, zREADELFARGS2, zREADELFARGS3);
  n += usage_seealso(o, name, args);
  n += usage_copyright(o, name, args);

  return ecode;
}

static int get_options_coredump(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    return usage(o, THIS_NAME, zREADELFARGS, ECODE_ARGUMENTS);
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
      } else {
        imode_t action = get_options2(o, zREADELFARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 2);
        }
        o->action |= action;
      }
    } else if ('-' == argv[i][0]) {
    } else if (0 == o->inpname[0]) {
      xstrncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    } else {
      return odeath(o, THIS_NAME, argv[i]);
    }
  }

  o->ocdump |= get_ocdump(o, zREADELFSWAP, o->action);

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, THIS_NAME, zREADELFARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage(o, THIS_NAME, zREADELFARGS, ECODE_OK);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int x = ECODE_MALLOC;
  poptions_t o = omalloc();
  if (o) {
    x = get_options_coredump(o, argc - 1, argv + 1, argv[0]);
    if (ECODE_ISOK(x) && o->inpname[0]) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        if (isELF(p)) {
          x = coredump(p, o);
        } else {
          printf_e("'%s': invalid file format.", o->inpname);
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

