#include <string.h>

#include "buffer.h"
#include "printf.h"
#include "opcode.h"
#include "options.h"
#include "scripts.h"
#include "objutils.h"

#include "static/usage.ci"
#include "static/objdebug-ng.ci"
#include "static/objdisassemble-ng.ci"

#define VERSION_VALUE "0.0"

static int usage_name(poptions_t o, const char* name, const args_t args[], const char* desc) {
  int n = 0;
  n += printf_text("NAME", USE_LT | USE_EOL);
  n += printf_text(name, USE_LT | USE_TAB | USE_COLON);
  n += printf_text(desc, USE_LT | USE_SPACE | USE_EOL);
  n += printf_eol();

  return n;
}

static int usage_synopsis0(poptions_t o, const char* name, const args_t args[]) {
  MALLOCA(char, m, 1024);

  int n0 = 0;
  int n1 = 0;
  if (args) {
    n0 += printf_text("SYNOPSIS", USE_LT | USE_EOL);
    n1  = printf_text(name, USE_LT | USE_TAB);
    if (args[0].option1 && args[0].option2) {
      snprintf(m, sizeof(m), "-%c|%s", args[0].option1, args[0].option2);
    } else if (args[0].option1) {
      snprintf(m, sizeof(m), "-%c", args[0].option1);
    } else if (args[0].option2) {
      snprintf(m, sizeof(m), "%s", args[0].option2);
    }
    n0 += printf_text(m, USE_LT | USE_SPACE | USE_SB | USE_EOL);

    for (int j = 1; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
      if (args[j].option1 && args[j].option2) {
         snprintf(m, sizeof(m), "-%c|%s", args[j].option1, args[j].option2);
      } else if (args[j].option1) {
        snprintf(m, sizeof(m), "-%c", args[j].option1);
      } else if (args[j].option2) {
        snprintf(m, sizeof(m), "%s", args[j].option2);
      }
      printf_pack(n1);
      n0 += printf_text(m, USE_LT | USE_SPACE | USE_SB | USE_EOL);
    }
  }

  return n0 + n1;
}

static int usage_synopsis1(poptions_t o, const char* name, const args_t args[], const char* more0, const char* more1) {
  MALLOCA(char, m, 1024);

  int n = 0;
  int z = strlen(name) + 2;
  if (more0 && args) {
    int x = snprintf(m, sizeof(m), "%s|[", more0);
    for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
      x += snprintf(m + x, sizeof(m) - x, "%c", args[j].option1);
    }

    x += snprintf(m + x, sizeof(m) - x, "]|");
    n += printf_pack(z);
    n += printf_text(m, USE_LT | USE_SPACE | USE_SBLT | USE_EOL);
  }

  if (more1 && args) {
    int x = snprintf(m, sizeof(m), "%s|[%s", more1, args[0].option2);
    for (int j = 1; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
      x += snprintf(m + x, sizeof(m) - x, ",%s", args[j].option2);
    }

    x += snprintf(m + x, sizeof(m) - x, "]");
    n += printf_pack(z);
    n += printf_text(m, USE_LT | USE_SPACE | USE_SBRT | USE_EOL);
  }

  printf_eol();

  return n;
}

static int usage_synopsis2(poptions_t o, const char* name, const args_t args[], const char* more0, const char* more1) {
  MALLOCA(char, m, 1024);

  int n = 0;
  if (more0 && more1 && args) {
    snprintf(m, sizeof(m), "%s options|%s=options", more0, more1);
    n += printf_text(m, USE_LT | USE_SPACE | USE_SB | USE_EOL);
  }

  n += printf_eol();

  return n;
}

static int usage_description(poptions_t o, const char* name, const args_t args[]) {
  int n = 0;
  n += printf_text("DESCRIPTION", USE_LT | USE_EOL);
  n += printf_eol();

  return n;
}

static int usage_options0(poptions_t o, const char* name, const args_t args[]) {
  MALLOCA(char, m, 1024);

  int n = 0;
  if (args) {
    n += printf_text("OPTIONS", USE_LT | USE_EOL);
    for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
      if (args[j].option1) {
        n += printf_nice(args[j].option1, USE_CHAR | USE_TAB | USE_DASH | USE_EOL);
      }
      if (args[j].option2) {
        n += printf_text(args[j].option2, USE_LT | USE_TAB | USE_EOL);
      }
      if (0 != args[j].content) {
        n += printf_pack(4);
        n += printf_text(args[j].content, USE_LT | USE_EOL);
      }

      if (isbits(args[j].action)) {
        n += printf_pack(4);
        n += printf_text("Equivalent to specifying", USE_LT | USE_COLON);
        for (int k = 0; (0 != args[k].option1) || (0 != args[k].option2); ++k) {
          if (k != j && !isbits(args[k].action) && (args[k].action & args[j].action)) {
            n += printf_text(args[k].option2, USE_LT | USE_SPACE);
          }
        }
        n += printf_eol();
      } else if (name && MODE_ISLOCKED(args[j].action, OPTPROGRAM_HELP)) {
        snprintf(m, sizeof(m), "Print a summary of the options to %s and exit.", name);
        n += printf_text(m, USE_LT | USE_TAB | USE_EOL);
      } else if (name && MODE_ISLOCKED(args[j].action, OPTPROGRAM_VERSION)) {
        snprintf(m, sizeof(m), "Print the version number of %s and exit.", name);
        n += printf_text(m, USE_LT | USE_TAB | USE_EOL);
      }
      n += printf_eol();
    }
  }

  return n;
}

static int usage_options1(poptions_t o, const char* name, const args_t args[], const char* more0, const char* more1) {
  MALLOCA(char, m, 1024);

  int n = 0;
  if (more0 && args) {
    int x = snprintf(m, sizeof(m), "%s|[", more0);
    for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
      x += snprintf(m + x, sizeof(m) - x, "%c", args[j].option1);
    }

    x += snprintf(m + x, sizeof(m) - x, "]");
    n += printf_text(m, USE_LT | USE_TAB | USE_EOL);
  }

  if (more1 && args) {
    int x = snprintf(m, sizeof(m), "%s|[=%s", more1, args[0].option2);
    for (int j = 1; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
      x += snprintf(m + x, sizeof(m) - x, ",=%s", args[j].option2);
    }

    x += snprintf(m + x, sizeof(m) - x, "]");
    n += printf_text(m, USE_LT | USE_TAB | USE_EOL);
    n += printf_eol();
  }

  if (args) {
    for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
      if (args[j].option1) {
        n += printf_nice(args[j].option1, USE_CHAR | USE_TAB | USE_DQ | USE_EOL);
      }
      if (args[j].option2) {
        n += printf_text(args[j].option2, USE_LT | USE_TAB | USE_DQEQ | USE_EOL);
      }
      if (0 != args[j].content) {
        n += printf_pack(4);
        n += printf_text(args[j].content, USE_LT | USE_EOL);
      }
      n += printf_eol();
    }
  }

  return n;
}

static int usage_options2(poptions_t o, const char* name, const args_t args[], const char* more0, const char* more1) {
  MALLOCA(char, m, 1024);

  int n = 0;
  if (more0 && args) {
    n += printf_yoke(more0, " options", USE_LT | USE_TAB | USE_EOL);
    n += printf_yoke(more1, "=options", USE_LT | USE_TAB | USE_EOL);
    n += printf_eol();

    for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
      if (args[j].option1) {
        n += printf_nice(args[j].option1, USE_CHAR | USE_TAB | USE_DQ | USE_EOL);
      }
      if (args[j].option2) {
        n += printf_text(args[j].option2, USE_LT | USE_TAB | USE_DQ | USE_EOL);
      }
      if (0 != args[j].content) {
        n += printf_pack(4);
        n += printf_text(args[j].content, USE_LT | USE_EOL);
      }
      if (args[j].groups != args[j + 1].groups) {
        n += printf_eol();
      }
    }

    n += printf_eol();
  }

  return n;
}

static int usage_seealso(poptions_t o, const char* name, const args_t args[]) {
  int n = 0;
  n += printf_text("SEE ALSO", USE_LT | USE_EOL);
  n += printf_book(zALLSEE, USE_LT | USE_TAB | USE_EOL);
  n += printf_eol();

  return n;
}

static int usage_version(poptions_t o, const char* name, const args_t args[]) {
  int n = 0;
  n += printf_yoke(name, " v"VERSION_VALUE, USE_LT | USE_EOL);
  n += printf_eol();

  return n;
}

static int usage_copyright(poptions_t o, const char* name, const args_t args[]) {
  int n = 0;
  n += printf_text("COPYRIGHT", USE_LT | USE_EOL);
  n += printf_book(zLICENSE, USE_LT | USE_TAB | USE_EOL);
  n += printf_eol();

  return n;
}

int odeath(poptions_t o, const char* name, const char* argument) {
  int n = 0;
  n += printf_text(name, USE_LT | USE_COLON);
  n += printf_text("invalid option --", USE_LT | USE_SPACE);
  n += printf_text(argument, USE_LT | USE_SPACE | USE_SQ);
  n += printf_eol();

  return ECODE_ARGUMENTS;
}

int usage0(poptions_t o, const char* name, const args_t args[], const int ecode) {
  int n = 0;
  n += usage_name(o, name, args, zDESCRIPTION);
  n += usage_synopsis0(o, name, args);
  n += usage_description(o, name, args);
  n += usage_options0(o, name, args);
  n += usage_seealso(o, name, args);
  n += usage_copyright(o, name, args);

  return ecode;
}

int usage1(poptions_t o, const char* name, const args_t args0[],
                     const char* more0, const char* more1, const int ecode) {
  int n = 0;
  n += usage_name(o, name, args0, zDESCRIPTION);
  n += usage_synopsis0(o, name, args0);
  n += usage_synopsis1(o, name, zDEBUGELFARGS, more0, more1);
  n += usage_description(o, name, args0);
  n += usage_options0(o, name, args0);
  n += usage_options1(o, name, zDEBUGELFARGS, more0, more1);
  n += usage_seealso(o, name, args0);
  n += usage_copyright(o, name, args0);

  return ecode;
}

int usage2(poptions_t o, const char* name, const args_t args0[],
                     const char* more0, const char* more1, const char* more2, const char* more3,
                     const int ecode) {
  int n = 0;
  n += usage_name(o, name, args0, zDESCRIPTION);
  n += usage_synopsis0(o, name, args0);
  n += usage_synopsis1(o, name, zDEBUGELFARGS, more0, more1);
  n += usage_synopsis2(o, name, zDISASSEMBLEARGS, more2, more3);
  n += usage_description(o, name, args0);
  n += usage_options0(o, name, args0);
  n += usage_options1(o, name, zDEBUGELFARGS, more0, more1);
  n += usage_options2(o, name, zDISASSEMBLEARGS, more2, more3);
  n += usage_seealso(o, name, args0);
  n += usage_copyright(o, name, args0);

  return ecode;
}

int usage3(poptions_t o, const char* name, const args_t args0[],
                     const char* more2, const char* more3) {
  int n = 0;
  n += usage_name(o, name, args0, zDESCRIPTION);
  n += usage_synopsis0(o, name, args0);
  n += usage_synopsis2(o, name, zDISASSEMBLEARGS, more2, more3);
  n += usage_description(o, name, args0);
  n += usage_options0(o, name, args0);
  n += usage_options2(o, name, zDISASSEMBLEARGS, more2, more3);
  n += usage_seealso(o, name, args0);
  n += usage_copyright(o, name, args0);

  return n;
}

int version0(poptions_t o, const char* name, const args_t args[]) {
  int n = 0;
  n += usage_version(o, name, args);
  n += usage_copyright(o, name, args);

  return ECODE_OK;
}

imode_t get_options1(poptions_t o, const args_t args[], const char *argv) {
  imode_t action = 0;
  for (int k = 1; k < strlen(argv); ++k) {
    imode_t action0 = 0;
    for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
      if (argv[k] == args[j].option1) {
        action0 = args[j].action;
        break;
      }
    }

    if (0 == action0) {
printf_w("TBD - %c", argv[k]);
// return 0;
    }

    action |= action0;
  }

  return action;
}

imode_t set_options1(poptions_t o, const args_t args[]) {
  imode_t action = 0;
  for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
    action |= args[j].action;
  }

  o->action |= action;
  return action;
}

imode_t get_options2(poptions_t o, const args_t args[], const char *argv) {
  imode_t action = 0;
  for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
    if (args[j].option2 && 0 == strcmp(argv, args[j].option2)) {
      action |= args[j].action;
    }
  }

  return action;
}

imode_t get_ocdump(poptions_t o, const imodeswap_t args[], imode_t action) {
  imode_t ocdump = 0;
  for (int j = 0; (0 != args[j].mode1) || (0 != args[j].mode2); ++j) {
    if (MODE_ISANY(args[j].mode1, action)) {
      ocdump |= args[j].mode2;
    }
  }

  return ocdump;
}

int breakup_args(char* args, char* dst0, const size_t dst0size, char* dst1, const size_t dst1size) {
  MALLOCA(char, tmp, 1024);
  strncpy(tmp, args, NELEMENTS(tmp));

  const char DELIMITS[] = "=";
  char* tok = strtok(tmp, DELIMITS);
  if (tok) {
    strncpy(dst0, tok, dst0size);
    tok = strtok(NULL, DELIMITS);
    if (tok) {
      strncpy(dst1, tok, dst1size);
      return ECODE_OK;
    }
  }

  return ECODE_GENERIC;
}

int isactions(handle_t p) {
  return ismode(p, MODE_ACTIONS);
}

int isoptions(handle_t p) {
  return ismode(p, MODE_OPTIONS);
}

handle_t amalloc() {
  paction_t p = xmalloc(sizeof(action_t));
  return setmode(p, MODE_ACTIONS);
}

handle_t omalloc() {
  poptions_t p = xmalloc(sizeof(options_t));
  if (p) {
    p->saddress = OPCODE_NULLADDR; /* --start-address */
    p->eaddress = OPCODE_NULLADDR; /* --stop-address */
  }

  return setmode(p, MODE_OPTIONS);
}

handle_t afree(handle_t p) {
  if (isactions(p)) {
    afree(CAST(paction_t, p)->actions);
    xfree(p);
    return NULL;
  }

  return p;
}

handle_t ofree(handle_t p) {
  if (isoptions(p)) {
    afree(CAST(poptions_t, p)->actions);
    xfree(p);
    return NULL;
  }

  return p;
}

int oinsert(handle_t o, handle_t p, const int action) {
  if (isoptions(o) && isactions(p)) {
    poptions_t o0 = CAST(poptions_t, o);
    paction_t p0 = CAST(paction_t, p);
    p0->action  = action;

    if (o0->actions) {
      paction_t p1 = o0->actions;
      while (p1->actions) {
        p1 = p1->actions;
      }

      p1->actions = p0;
    } else {
      o0->actions = p0;
    }

    return ECODE_OK;
  }

  return ECODE_HANDLE;
}

int oinsertvalue(handle_t o, const int action, const uint64_t value) {
  if (isoptions(o)) {
    paction_t p = amalloc();
    if (isactions(p)) {
      p->value = value;
      oinsert(o, p, action);
      return ECODE_OK;
    }
  }

  return ECODE_HANDLE;
}

int oinsertsecname(handle_t o, const int action, const char *secname) {
  if (isoptions(o)) {
    paction_t p = amalloc();
    if (isactions(p)) {
      strncpy(p->secname, secname, NELEMENTS(p->secname));
      oinsert(o, p, action);
      return ECODE_OK;
    }
  }

  return ECODE_HANDLE;
}

