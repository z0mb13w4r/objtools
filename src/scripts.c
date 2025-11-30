#include "scripts.h"
#include "objutils.h"

static imode_t breakup_script(const pargs_t p, const char *name, uint64_t *uvalue, char *svalue) {
  MALLOCA(char, tmp, 1024);

  int x = ECODE_ARGUMENTS;
  if (p && name && uvalue && svalue) {
    xstrncpy(tmp, name, NELEMENTS(tmp));

    const char DELIMITS[] = "(){},";
    char* tok = strtok(tmp, DELIMITS);
    if (tok) {
      for (pargs_t pp = p; 0 != pp->option2; ++pp) {
        if (0 == xstrcmp(pp->option2, tok)) {
          x = pp->action;
          break;
        }
      }

      if (ECODE_ARGUMENTS != x) {
        tok = strtok(NULL, DELIMITS);
        if (tok) {
          *uvalue = atovalue(tok);
          xstrcpy(svalue, tok);
        }
      }
    }
  }

  return x;
}

static pick_t zSTRINGPARAM[] = {
  ACT_VIGENERED,
  ACT_VIGENEREE,
  PICK_END
};

static int sprocess(poptions_t o, const char *script, const pargs_t args0, const pargs_t args1) {
  MALLOCA(char, tmp, 1024);
  if (isoptions(o) && script && script[0]) {
    xstrncpy(tmp, script, NELEMENTS(tmp));

    const char DELIMIT = ';';
    char *p0 = tmp;
    char *p1 = strchr(p0, DELIMIT);
    if (p1) {
      while (p1) {
        *p1 = 0;
        char n[1024];
        uint64_t v = 0;
        const int x0 = breakup_script(args0, p0, &v, n);
        if (ECODE_ARGUMENTS != x0) {
          if (isused(zSTRINGPARAM, x0)) {
            oinsertsname(o, x0, n);
          } else {
            oinsertvalue(o, x0, v);
          }
        } else if (args1) {
          const int x1 = breakup_script(args1, p0, &v, n);
          if (ECODE_ARGUMENTS != x1) {
            if (isused(zSTRINGPARAM, x0)) {
              oinsertsname(o, x0, n);
            } else {
              oinsertvalue(o, x1, v);
            }
          }
        }

        p0 = p1 + 1;
        p1 = strchr(p0, DELIMIT);
      }

      return ECODE_OK;
    } else {
      char n[1024];
      uint64_t v = 0;
      const int x0 = breakup_script(args0, script, &v, n);
      if (ECODE_ARGUMENTS != x0) {
        if (isused(zSTRINGPARAM, x0)) {
          return oinsertsname(o, x0, n);
        } else {
          return oinsertvalue(o, x0, v);
        }
      } else if (args1) {
        const int x1 = breakup_script(args1, script, &v, n);
        if (ECODE_ARGUMENTS != x1) {
          if (isused(zSTRINGPARAM, x1)) {
            return oinsertsname(o, x1, n);
          } else {
            return oinsertvalue(o, x1, v);
          }
        }
      }
    }
  }

  return ECODE_ARGUMENTS;
}

int sinsert(poptions_t o, const char *script, const pargs_t args0, const pargs_t args1) {
  MALLOCA(char, tmp, 1024);
  if (isoptions(o) && script && script[0]) {
    if ('@' == script[0]) {
      FILE *fp = fopen(script + 1, "rt");
      if (fp) {
        while (fgets(tmp, NELEMENTS(tmp), fp)) {
          sprocess(o, tmp, args0, args1);
        }
        fclose(fp);
        return ECODE_OK;
      }
    } else {
      return sprocess(o, script, args0, args1);
    }
  }

  return ECODE_HANDLE;
}

