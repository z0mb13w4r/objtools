#include "buffer.h"
#include "printf.h"
#include "readar.h"
#include "scripts.h"
#include "readelf.h"
#include "options.h"

#include "static/readelf-ng.ci"

#define THIS_NAME "readelf-ng"

static int get_options_readelf(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    return usage2(o, THIS_NAME, zREADELFARGS, zREADELFARGS0, zREADELFARGS1, zREADELFARGS2, zREADELFARGS3, ECODE_ARGUMENTS);
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == strcmp(arg0, zREADELFARGS1)) {
          imode_t ocdump = get_options2(o, zDEBUGELFARGS, arg1);
          if (0 == ocdump) {
            return odeath(o, THIS_NAME, arg1);
          }
          o->ocdump |= ocdump;
        } else if (0 == strcmp(arg0, "--hex-dump")) {
          oinsertsecname(o, ACT_HEXDUMP, arg1);
        } else if (0 == strcmp(arg0, "--string-dump")) {
          oinsertsecname(o, ACT_STRDUMP8, arg1);
        } else if (0 == strcmp(arg0, "--unicode-dump")) {
          oinsertsecname(o, ACT_STRDUMP16, arg1);
        } else if (0 == strcmp(arg0, "--relocated-dump")) {
          oinsertsecname(o, ACT_RELDUMP, arg1);
        } else if (0 == strcmp(arg0, "--code-dump")) {
          oinsertsecname(o, ACT_CODEDUMP, arg1);
        } else if (0 == strcmp(arg0, "--disassemble")) {
          oinsertsecname(o, ACT_DISASSEMBLE, arg1);
        } else if (0 == strcmp(arg0, "--decompress")) {
          oinsertsecname(o, ACT_ZLIB, arg1);
        } else if (0 == strcmp(arg0, "--script")) {
          sinsert(o, arg1);
        } else {
          return odeath(o, THIS_NAME, arg0 + 2);
        }
      } else {
        imode_t action = get_options2(o, zREADELFARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 2);
        }
        o->action |= action;
      }
    } else if ('-' == argv[i][0]) {
      if (argv[i][0] == zREADELFARGS0[0] && argv[i][1] == zREADELFARGS0[1]) {
        imode_t ocdump = get_options1(o, zDEBUGELFARGS, argv[i] + 1);
        if (0 == ocdump && argv[i][2]) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        o->ocdump |= ocdump ? ocdump : set_options1(o, zDEBUGELFARGS);
      } else if (0 == strcmp(argv[i], "-x")) {
        oinsertsecname(o, ACT_HEXDUMP, argv[++i]);
      } else if (0 == strcmp(argv[i], "-p")) {
        oinsertsecname(o, ACT_STRDUMP8, argv[++i]);
      } else if (0 == strcmp(argv[i], "-U")) {
        oinsertsecname(o, ACT_STRDUMP16, argv[++i]);
      } else if (0 == strcmp(argv[i], "-R")) {
        oinsertsecname(o, ACT_RELDUMP, argv[++i]);
      } else if (0 == strcmp(argv[i], "-Z")) {
        oinsertsecname(o, ACT_CODEDUMP, argv[++i]);
      } else if (0 == strcmp(argv[i], "-C")) {
        oinsertsecname(o, ACT_DISASSEMBLE, argv[++i]);
      } else if (0 == strcmp(argv[i], "-z")) {
        oinsertsecname(o, ACT_ZLIB, argv[++i]);
      } else if (0 == strcmp(argv[i], "-T")) {
        sinsert(o, argv[++i]);
      } else {
        imode_t action = get_options1(o, zREADELFARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        o->action |= action;
      }
    } else {
      strncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, THIS_NAME, zREADELFARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage2(o, THIS_NAME, zREADELFARGS, zREADELFARGS0, zREADELFARGS1, zREADELFARGS2, zREADELFARGS3, ECODE_OK);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int x = ECODE_MALLOC;
  poptions_t o = omalloc();
  if (o) {
    x = get_options_readelf(o, argc - 1, argv + 1, argv[0]);
    if (ECODE_ISOK(x) && o->inpname[0]) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        if (isAR(p)) {
          x = readar(p, o);
        } else if (isELF(p)) {
          x = readelf(p, o);
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

