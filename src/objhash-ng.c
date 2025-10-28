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

#define THIS_NAME "objhash-ng"

static int usage(poptions_t o, const char* name, const args_t args[], const int ecode) {
  int n = 0;
  n += usage_name(o, name, args, zDESCRIPTION);
  n += usage_synopsis0(o, name, args);
  n += usage_description(o, name, args, NULL);
  n += usage_options0(o, name, args);
  n += usage_seealso(o, name, args);
  n += usage_copyright(o, name, args);

  return ecode;
}

static int get_options_objhash(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 1) {
    return usage(o, THIS_NAME, OBJHASHARGS, ECODE_ARGUMENTS);
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 256);
      MALLOCA(char, arg1, 256);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == xstrcmp(arg0, "--hex-dump")) {
          oinsertsecname(o, ACT_HEXDUMP, arg1);
        } else if (0 == xstrcmp(arg0, "--string-dump")) {
          oinsertsecname(o, ACT_STRDUMP8, arg1);
        } else if (0 == xstrcmp(arg0, "--convert")) {
          o->convert = atol(arg1);
        } else {
          return odeath(o, THIS_NAME, arg0 + 2);
        }
      } else {
        imode_t action = get_options2(o, OBJHASHARGS, argv[i]);
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
      } else if (0 == xstrcmp(argv[i], "-C")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        o->convert = atoimode(argv[++i]);
      } else {
        imode_t action = get_options1(o, OBJHASHARGS, argv[i]);
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

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, THIS_NAME, OBJHASHARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage(o, THIS_NAME, OBJHASHARGS, ECODE_OK);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int x = ECODE_MALLOC;
  poptions_t o = omalloc();
  if (o) {
    x = get_options_objhash(o, argc - 1, argv + 1, argv[0]);
    if (ECODE_ISOK(x) && o->inpname[0]) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        x = objhash(p, o);
      } else {
        printf_e("'%s': no such file.", o->inpname);
      }

      bfree(p);
    }

    ofree(o);
  }

  return x;
}

