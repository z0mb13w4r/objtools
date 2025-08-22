#include "buffer.h"
#include "printf.h"
#include "readar.h"
#include "readelf.h"
#include "options.h"

#include "static/objdwarf-ng.ci"

static int get_options_objdwarf(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    usage1(o, "objdwarf-ng", zOBJDWARFARGS, zOBJDWARFARGS0, zOBJDWARFARGS1);
    return -1;
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == strcmp(arg0, zOBJDWARFARGS1)) {
          o->ocdump |= get_options2(o, zDEBUGELFARGS, arg1);
        } else if (0 == strcmp(arg0, "--hex-dump")) {
          oinsertsecname(o, ACT_HEXDUMP, arg1);
        } else if (0 == strcmp(arg0, "--string-dump")) {
          oinsertsecname(o, ACT_STRDUMP8, arg1);
        } else if (0 == strcmp(arg0, "--decompress")) {
          oinsertsecname(o, ACT_ZLIB, arg1);
        }
      } else {
        o->action |= get_options2(o, zOBJDWARFARGS, argv[i]);
      }
    } else if ('-' == argv[i][0]) {
      if (0 == strcmp(argv[i], zOBJDWARFARGS0)) {
        imode_t ocdump = get_options1(o, zDEBUGELFARGS, argv[i] + 1);
        o->ocdump |= ocdump ? ocdump : set_options1(o, zDEBUGELFARGS);
      } else if (0 == strcmp(argv[i], "-x")) {
        oinsertsecname(o, ACT_HEXDUMP, argv[++i]);
      } else if (0 == strcmp(argv[i], "-p")) {
        oinsertsecname(o, ACT_STRDUMP8, argv[++i]);
      } else if (0 == strcmp(argv[i], "-z")) {
        oinsertsecname(o, ACT_ZLIB, argv[++i]);
      } else {
        o->action |= get_options1(o, zOBJDWARFARGS, argv[i]);
      }
    } else {
      strncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, "objdwarf-ng", zOBJDWARFARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage1(o, "objdwarf-ng", zOBJDWARFARGS, zOBJDWARFARGS0, zOBJDWARFARGS1);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = omalloc();
  if (o) {
    r = get_options_objdwarf(o, argc - 1, argv + 1, argv[0]);
    if (0 == r && o->inpname[0]) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        if (isAR(p)) {
          r = readar(p, o);
        } else if (isELF(p)) {
          r = readelf(p, o);
        }
      }

      nfree(p);
    }
  }

  nfree(o);
  return r;
}

