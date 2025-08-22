#include "buffer.h"
#include "printf.h"
#include "objhash.h"
#include "options.h"

static const args_t OBJHASHARGS[] = {
  {'H', "--help",              OPTPROGRAM_HELP},
  {'V', "--version",           OPTPROGRAM_VERSION},
  {'s', "--sections",          OPTOBJHASH_SECTIONS},
  {'h', "--headers",           OPTOBJHASH_HEADERS},
  {'a', "--all",               OPTOBJHASH_ALL},
  {0, NULL}
};

static int get_options_objhash(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 1) {
    return -1;
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 256);
      MALLOCA(char, arg1, 256);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == strcmp(arg0, "--hex-dump")) {
          oinsertsecname(o, ACT_HEXDUMP, arg1);
        } else if (0 == strcmp(arg0, "--string-dump")) {
          oinsertsecname(o, ACT_STRDUMP8, arg1);
        } else if (0 == strcmp(arg0, "--convert")) {
          o->convert = atol(arg1);
        }
      } else {
        o->action |= get_options2(o, OBJHASHARGS, argv[i]);
      }
    } else if ('-' == argv[i][0]) {
      if (0 == strcmp(argv[i], "-x")) {
        oinsertsecname(o, ACT_HEXDUMP, argv[++i]);
      } else if (0 == strcmp(argv[i], "-p")) {
        oinsertsecname(o, ACT_STRDUMP8, argv[++i]);
      } else if (0 == strcmp(argv[i], "-C")) {
        o->convert = atoimode(argv[++i]);
      } else {
        o->action |= get_options1(o, OBJHASHARGS, argv[i]);
      }
    } else if (0 == o->inpname0[0]) {
      strncpy(o->inpname0, argv[i], NELEMENTS(o->inpname0));
    } else if (0 == o->inpname1[0]) {
      strncpy(o->inpname1, argv[i], NELEMENTS(o->inpname1));
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, "objhash-ng", OBJHASHARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage0(o, "objhash-ng", OBJHASHARGS);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = omalloc();
  if (o) {
    r = get_options_objhash(o, argc - 1, argv + 1, argv[0]);
    if (0 == r) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        r = objhash(p, o);
      }

      nfree(p);
    }
  }

  nfree(o);
  return r;
}

