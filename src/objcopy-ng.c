#include "buffer.h"
#include "printf.h"
#include "objcopy.h"
#include "options.h"

#include "static/objcopy-ng.ci"

#define THIS_NAME "objcopy-ng"

static int usage(poptions_t o, const char* name, const args_t args[], const int ecode) {
  int n = 0;
  n += usage_name(o, name, args, zDESCRIPTION);
  n += usage_synopsis0(o, name, args);
  n += usage_description(o, name, args, zCOPYDESCRIPTION);
  n += usage_options0(o, name, args);
  n += usage_seealso(o, name, args);
  n += usage_copyright(o, name, args);

  return ecode;
}

static int get_options_objcopy(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 1) {
    return usage(o, THIS_NAME, zOBJCOPYARGS, ECODE_ARGUMENTS);
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      if (0 == xstrcmp(argv[i], "--add-section")) {
        paction_t p = amalloc();
        if (p && ECODE_ISOK(breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname)))) {
          oinsert(o, p, ACT_ADDSECTION);
        }
      } else if (0 == xstrcmp(argv[i], "--dump-section")) {
        paction_t p = amalloc();
        if (p && ECODE_ISOK(breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname)))) {
          oinsert(o, p, ACT_DUMPSECTION);
        }
      } else if (0 == xstrcmp(argv[i], "--update-section")) {
        paction_t p = amalloc();
        if (p && ECODE_ISOK(breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname)))) {
          oinsert(o, p, ACT_UPDATESECTION);
        }
      } else {
        imode_t action = get_options2(o, zOBJCOPYARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 2);
        }
        o->action |= action;
      }
    } else if ('-' == argv[i][0]) {
      imode_t action = get_options1(o, zOBJCOPYARGS, argv[i]);
      if (0 == action) {
        return odeath(o, THIS_NAME, argv[i] + 1);
      }
      o->action |= action;
    } else if (0 == o->inpname[0]) {
      xstrncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    } else {
      return odeath(o, THIS_NAME, argv[i]);
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, THIS_NAME, zOBJCOPYARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage(o, THIS_NAME, zOBJCOPYARGS, ECODE_OK);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int x = ECODE_MALLOC;
  poptions_t o = omalloc();
  if (o) {
    x = get_options_objcopy(o, argc - 1, argv + 1, argv[0]);
    if (ECODE_ISOK(x) && o->inpname[0]) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        x = objcopy(p, o);
      } else {
        printf_e("'%s': no such file.", o->inpname);
      }

      bfree(p);
    }

    ofree(o);
  }

  return x;
}

