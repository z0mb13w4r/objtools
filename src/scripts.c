#include "scripts.h"
#include "objutils.h"

static imode_t breakup_script(const pargs_t p, const char *name, uint64_t *value) {
  MALLOCA(char, tmp, 1024);

  int x = -1;
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

      if (-1 != x) {
        tok = strtok(NULL, DELIMITS);
        if (tok) {
          *value = atovalue(tok);
        }
      }
    }
  }

  return x;
}

int sprocess(poptions_t o, const char *script) {
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
        const int x = breakup_script(zSCRIPTCOMMANDS, p0, &v);
        if (-1 != x) {
          oinsertvalue(o, x, v);
        }

        p0 = p1 + 1;
        p1 = strchr(p0, DELIMIT);
      }

      return 0;
    } else {
      uint64_t v = 0;
      const int x = breakup_script(zSCRIPTCOMMANDS, script, &v);
      if (-1 != x) {
        return oinsertvalue(o, x, v);
      }
    }
  }

  return ECODE_HANDLE;
}

int sinsert(poptions_t o, const char *script) {
  MALLOCA(char, tmp, 1024);
  if (isoptions(o) && script && script[0]) {
    if ('@' == script[0]) {
      FILE *fp = fopen(script + 1, "rt");
      if (fp) {
        while (fgets(tmp, NELEMENTS(tmp), fp)) {
          sprocess(o, tmp);
        }
        fclose(fp);
        return ECODE_OK;
      }
    } else {
      return sprocess(o, script);
    }
  }

  return ECODE_HANDLE;
}

