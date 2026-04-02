#include "buffer.h"
#include "printf.h"
#include "detect.h"
#include "options.h"
#include "objutils.h"

#include "static/detect-ng.ci"

#define THIS_NAME "detect-ng"

static int usage(poptions_t o, const char* name, const args_t args[], const int ecode) {
  int n = 0;
  n += usage_name(o, name, args, zDESCRIPTIONPE);
  n += usage_synopsis0(o, name, args);
  n += printf_eol();
  n += usage_description(o, name, args, zPEDESCRIPTION);
  n += usage_options0(o, name, args);
  n += usage_seealso(o, name, args);
  n += usage_copyright(o, name, args);

  return ecode;
}

static int get_options_detect(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    return usage(o, THIS_NAME, zDETECTARGS, ECODE_ARGUMENTS);
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      imode_t action = get_options2(o, zDETECTARGS, argv[i]);
      if (0 == action) {
        return odeath(o, THIS_NAME, argv[i] + 2);
      }
      o->action |= action;
    } else if ('-' == argv[i][0]) {
      imode_t action = get_options1(o, zDETECTARGS, argv[i]);
      if (0 == action) {
        return odeath(o, THIS_NAME, argv[i] + 1);
      }
      o->action |= action;
    } else if (0 == o->inpname0[0]) {
      xstrncpy(o->inpname0, argv[i], NELEMENTS(o->inpname0));
    } else if (0 == o->inpname1[0]) {
      xstrncpy(o->inpname1, argv[i], NELEMENTS(o->inpname1));
    } else if (0 == o->outname0[0]) {
      xstrncpy(o->outname0, argv[i], NELEMENTS(o->outname0));
    } else {
      return odeath(o, THIS_NAME, argv[i]);
    }
  }

  if (MODE_ISANY(o->action, OPTPROGRAM_VERSION)) {
    return version0(o, THIS_NAME, zDETECTARGS);
  }

  if (MODE_ISANY(o->action, OPTPROGRAM_HELP)) {
    return usage(o, THIS_NAME, zDETECTARGS, ECODE_OK);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int x = ECODE_MALLOC;
  poptions_t o = omalloc();
  if (o) {
    x = get_options_detect(o, argc - 1, argv + 1, argv[0]);
    if (ECODE_ISOK(x) && o->inpname0[0]) {
      pbuffer_t p = bopen(o->inpname0);
      if (p) {
        if (isPE(p)) {
          x = detect(p, o);
        } else if (MODE_ISANY(o->action, OPTDETECT_CREATE)) {
          x = detect_create(p, o);
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

