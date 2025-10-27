#include "buffer.h"
#include "printf.h"
#include "readpe.h"
#include "options.h"
#include "scripts.h"
#include "objutils.h"

#include "static/readpe-ng.ci"

#define THIS_NAME "readpe-ng"

static int usage(poptions_t o, const char* name, const args_t args0[], const int ecode) {
  int n = 0;
  n += usage_name(o, name, args0, zDESCRIPTION);
  n += usage_synopsis0(o, name, args0);
  n += usage_synopsis2(o, name, zDISASSEMBLEARGS, zREADPEARGS2, zREADPEARGS3);
  n += printf_eol();
  n += usage_description(o, name, args0);
  n += usage_options0(o, name, args0);
  n += usage_options2(o, name, zDISASSEMBLEARGS, zREADPEARGS2, zREADPEARGS3);
  n += usage_seealso(o, name, args0);
  n += usage_copyright(o, name, args0);

  return ecode;
}

static int get_options_readpe(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    return usage(o, THIS_NAME, zREADPEARGS, ECODE_ARGUMENTS);
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == xstrcmp(arg0, "--hex-dump")) {
          oinsertsecname(o, ACT_HEXDUMP, arg1);
        } else if (0 == xstrcmp(arg0, "--string-dump")) {
          oinsertsecname(o, ACT_STRDUMP8, arg1);
        } else if (0 == xstrcmp(arg0, "--unicode-dump")) {
          oinsertsecname(o, ACT_STRDUMP16, arg1);
        } else if (0 == xstrcmp(arg0, "--relocated-dump")) {
          oinsertsecname(o, ACT_RELDUMP, arg1);
        } else if (0 == xstrcmp(arg0, "--code-dump")) {
          oinsertsecname(o, ACT_CODEDUMP, arg1);
        } else if (0 == xstrcmp(arg0, "--disassemble")) {
          oinsertsecname(o, ACT_DISASSEMBLE, arg1);
        } else if (0 == xstrcmp(arg0, "--decompress")) {
          oinsertsecname(o, ACT_ZLIB, arg1);
        } else if (0 == xstrcmp(arg0, "--script")) {
          sinsert(o, arg1);
        }
      } else {
        imode_t action = get_options2(o, zREADPEARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 2);
        }
        o->action |= action;
      }
    } else if ('-' == argv[i][0]) {
      if (0 == xstrcmp(argv[i], "-x")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsecname(o, ACT_HEXDUMP, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-p")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsecname(o, ACT_STRDUMP8, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-U")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsecname(o, ACT_STRDUMP16, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-R")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsecname(o, ACT_RELDUMP, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-Z")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsecname(o, ACT_CODEDUMP, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-C")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsecname(o, ACT_DISASSEMBLE, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-z")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsecname(o, ACT_ZLIB, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-T")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        sinsert(o, argv[++i]);
      } else {
        imode_t action = get_options1(o, zREADPEARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        o->action |= action;
      }
    } else if (0 == o->inpname[0]) {
      xstrncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    } else {
      return odeath(o, THIS_NAME, argv[i]);
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, THIS_NAME, zREADPEARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage(o, THIS_NAME, zREADPEARGS, ECODE_OK);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int x = ECODE_MALLOC;
  poptions_t o = omalloc();
  if (o) {
    x = get_options_readpe(o, argc - 1, argv + 1, argv[0]);
    if (ECODE_ISOK(x) && o->inpname[0]) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        if (isPE(p)) {
          x = readpe(p, o);
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

