#include "buffer.h"
#include "printf.h"
#include "scripts.h"
#include "options.h"
#include "coredump.h"

#include "static/coredump-ng.ci"

#define THIS_NAME "coredump-ng"

static int usage(poptions_t o, const char* name, const args_t args[], const int ecode) {
  int n = 0;
  n += usage_name(o, name, args, zCOREDESCRIPTION);
  n += usage_synopsis0(o, name, args);
//  n += usage_synopsis1(o, name, zDEBUGELFARGS, zREADELFARGS0, zREADELFARGS1);
  n += usage_synopsis2(o, name, zDISASSEMBLEARGS, zCOREELFARGS2, zCOREELFARGS3);
  n += usage_objname(o, name, "objfiles");
  n += printf_eol();
  n += usage_description(o, name, args, zELFDESCRIPTION);
  n += usage_options0(o, name, args);
//  n += usage_options1(o, name, zDEBUGELFARGS, zREADELFARGS0, zREADELFARGS1);
  n += usage_options2(o, name, zDISASSEMBLEARGS, zCOREELFARGS2, zCOREELFARGS3);
  n += usage_seealso(o, name, args);
  n += usage_copyright(o, name, args);

  return ecode;
}

static int get_options_coredump(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    return usage(o, THIS_NAME, zCOREELFARGS, ECODE_ARGUMENTS);
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  o->action = OPRPROGRAM_NOWARNINGS | OPRPROGRAM_COREDUMPDETAILED;

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == xstrcmp(arg0, zCOREELFARGS3)) {
          imode_t ocdump = get_options2(o, zDISASSEMBLEARGS, arg1);
          if (0 == ocdump) {
            return odeath(o, THIS_NAME, arg1);
          }
          o->ocdump |= ocdump;
        } else if (0 == xstrcmp(arg0, "--disassemble")) {
          oinsertsname(o, ACT_DISASSEMBLE, arg1);
        } else {
          return odeath(o, THIS_NAME, arg0 + 2);
        }
      } else {
        imode_t action = get_options2(o, zCOREELFARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 2);
        }
        o->action |= action;
      }
    } else if ('-' == argv[i][0]) {
      if ( argv[i][0] == zCOREELFARGS2[0] && argv[i][1] == zCOREELFARGS2[1]  ) {
        imode_t ocdump = get_options2(o, zDISASSEMBLEARGS, argv[++i]);
        if (0 == ocdump) {
          return odeath(o, THIS_NAME, argv[i]);
        }
        o->ocdump |= ocdump;
      } else if (0 == xstrcmp(argv[i], "-m")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsname(o, ACT_DISASSEMBLE, argv[++i]);
      } else {
        imode_t action = get_options1(o, zCOREELFARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        o->action |= action;
      }
    } else if (0 == o->inpname0[0]) {
      xstrncpy(o->inpname0, argv[i], NELEMENTS(o->inpname0));
    } else if (0 == o->inpname1[0]) {
      xstrncpy(o->inpname1, argv[i], NELEMENTS(o->inpname1));
    } else {
      return odeath(o, THIS_NAME, argv[i]);
    }
  }

  o->ocdump |= get_ocdump(o, zCOREELFSWAP, o->action);

  if (MODE_ISANY(o->action, OPTPROGRAM_VERSION)) {
    return version0(o, THIS_NAME, zCOREELFARGS);
  }

  if (MODE_ISANY(o->action, OPTPROGRAM_HELP)) {
    return usage(o, THIS_NAME, zCOREELFARGS, ECODE_OK);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int x = ECODE_MALLOC;
  poptions_t o = omalloc();
  if (o) {
    x = get_options_coredump(o, argc - 1, argv + 1, argv[0]);
    if (ECODE_ISOK(x) && o->inpname0[0]) {
      pbuffer_t p = bopen(o->inpname0);
      pbuffer_t q = bopen(o->inpname1);
      if (p) {
        if (isELFcore(p)) {
          x = coredump(p, q, o);
        } else {
          printf_e("'%s': invalid file format.", o->inpname0);
        }
      } else {
        printf_e("'%s': no such file.", o->inpname0);
      }

     bfree(p);
    }

    ofree(o);
  }

  return x;
}

