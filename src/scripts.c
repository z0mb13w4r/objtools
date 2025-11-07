#include "scripts.h"
#include "objutils.h"

static imode_t breakup_script(const pargs_t p, const char *name, uint64_t *value) {
  MALLOCA(char, tmp, 1024);

  int x = ECODE_ARGUMENTS;
  if (p && name && value) {
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
          *value = atovalue(tok);
        }
      }
    }
  }

  return x;
}

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
        uint64_t v = 0;
        const int x0 = breakup_script(args0, p0, &v);
        if (ECODE_ARGUMENTS != x0) {
          oinsertvalue(o, x0, v);
        } else if (args1) {
          const int x1 = breakup_script(args1, p0, &v);
          if (ECODE_ARGUMENTS != x1) {
            oinsertvalue(o, x1, v);
          }
        }

        p0 = p1 + 1;
        p1 = strchr(p0, DELIMIT);
      }

      return ECODE_OK;
    } else {
      uint64_t v = 0;
      const int x0 = breakup_script(args0, script, &v);
      if (ECODE_ARGUMENTS != x0) {
        return oinsertvalue(o, x0, v);
      } else if (args1) {
        const int x1 = breakup_script(args1, script, &v);
        if (ECODE_ARGUMENTS != x1) {
          return oinsertvalue(o, x1, v);
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

