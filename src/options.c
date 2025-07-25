#include <string.h>

#include "buffer.h"
#include "printf.h"
#include "opcode.h"
#include "options.h"
#include "scripts.h"
#include "objutils.h"

#include "static/usage.ci"
#include "static/convert-ng.ci"
#include "static/objdump-ng.ci"
#include "static/readelf-ng.ci"
#include "static/objdebug-ng.ci"
#include "static/objdwarf-ng.ci"
#include "static/objdisassemble-ng.ci"

#define VERSION_VALUE "0.0"

static const args_t OBJCOPYARGS[] = {
  {'H', "--help",                           OPTPROGRAM_HELP},
  {'V', "--version",                        OPTPROGRAM_VERSION},
  {0,   "--adjust-warnings",                OPTOBJCOPY_CHANGE_WARNINGS},
  {0,   "--change-warnings",                OPTOBJCOPY_CHANGE_WARNINGS},
  {0,   "--debugging",                      OPTOBJCOPY_DEBUGGING},
  {0,   "--decompress-debug-sections",      OPTOBJCOPY_DECOMPRESS_DEBUG_SECTIONS},
  {'U', "--disable-deterministic-archives", OPTOBJCOPY_DISABLE_DETERMINISTIC_ARCHIVES},
  {'x', "--discard-all",                    OPTOBJCOPY_DISCARD_ALL},
  {'X', "--discard-locals",                 OPTOBJCOPY_DISCARD_LOCALS},
  {'D', "--enable-deterministic-archives",  OPTOBJCOPY_ENABLE_DETERMINISTIC_ARCHIVES},
  {0,   "--extract-dwo",                    OPTOBJCOPY_EXTRACT_DWO},
  {0,   "--extract-symbol",                 OPTOBJCOPY_EXTRACT_SYMBOL},
  {0,   "--impure",                         OPTOBJCOPY_IMPURE},
  {0,   "--keep-file-symbols",              OPTOBJCOPY_KEEP_FILE_SYMBOLS},
  {0,   "--localize-hidden",                OPTOBJCOPY_LOCALIZE_HIDDEN},
  {'M', "--merge-notes",                    OPTOBJCOPY_MERGE_NOTES},
  {0,   "--no-merge-notes",                 OPTOBJCOPY_NO_MERGE_NOTES},
  {0,   "--no-adjust-warnings",             OPTOBJCOPY_NO_CHANGE_WARNINGS},
  {0,   "--no-change-warnings",             OPTOBJCOPY_NO_CHANGE_WARNINGS},
  {0,   "--only-keep-debug",                OPTOBJCOPY_ONLY_KEEP_DEBUG},
  {'p', "--preserve-dates",                 OPTOBJCOPY_PRESERVE_DATES},
  {0,   "--pure",                           OPTOBJCOPY_PURE},
  {0,   "--readonly-text",                  OPTOBJCOPY_READONLY_TEXT},
  {0,   "--strip-section-headers",          OPTOBJCOPY_STRIP_SECTION_HEADERS},
  {'S', "--strip-all",                      OPTOBJCOPY_STRIP_ALL},
  {'g', "--strip-debug",                    OPTOBJCOPY_STRIP_DEBUG},
  {0,   "--strip-dwo",                      OPTOBJCOPY_STRIP_DWO},
  {0,   "--strip-unneeded",                 OPTOBJCOPY_STRIP_UNNEEDED},
  {'v', "--verbose",                        OPTOBJCOPY_VERBOSE},
  {0,   "--weaken",                         OPTOBJCOPY_WEAKEN},
  {0,   "--writable-text",                  OPTOBJCOPY_WRITABLE_TEXT},
  {0,   "--dump-sections-all",              OPTOBJCOPY_DUMP_SECTIONS_ALL},
  {0, NULL},
};

static const args_t OBJHASHARGS[] = {
  {'H', "--help",              OPTPROGRAM_HELP},
  {'V', "--version",           OPTPROGRAM_VERSION},
  {'s', "--sections",          OPTOBJHASH_SECTIONS},
  {'h', "--headers",           OPTOBJHASH_HEADERS},
  {'a', "--all",               OPTOBJHASH_ALL},
  {0, NULL}
};

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

static int usage0(poptions_t o, const char* name, const args_t args[]) {
  int n = 0;
  n += usage_name(o, name, args, zDESCRIPTION);
  n += usage_synopsis0(o, name, args);
  n += usage_description(o, name, args);
  n += usage_options0(o, name, args);
  n += usage_seealso(o, name, args);
  n += usage_copyright(o, name, args);

  return n;
}

static int usage1(poptions_t o, const char* name, const args_t args0[],
                  const char* more0, const char* more1) {
  int n = 0;
  n += usage_name(o, name, args0, zDESCRIPTION);
  n += usage_synopsis0(o, name, args0);
  n += usage_synopsis1(o, name, zDEBUGELFARGS, more0, more1);
  n += usage_description(o, name, args0);
  n += usage_options0(o, name, args0);
  n += usage_options1(o, name, zDEBUGELFARGS, more0, more1);
  n += usage_seealso(o, name, args0);
  n += usage_copyright(o, name, args0);

  return 0;
}

static int usage2(poptions_t o, const char* name, const args_t args0[],
                  const char* more0, const char* more1, const char* more2, const char* more3) {
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

  return n;
}

static int version0(poptions_t o, const char* name, const args_t args[]) {
  int n = 0;
  n += usage_version(o, name, args);
  n += usage_copyright(o, name, args);

  return n;
}

static imode_t get_options1(poptions_t o, const args_t args[], const char *argv) {
  imode_t action = 0;
  for (int k = 1; k < strlen(argv); ++k) {
    for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
      if (argv[k] == args[j].option1) {
        action |= args[j].action;
        break;
      }
    }
  }

  return action;
}

static imode_t set_options1(poptions_t o, const args_t args[]) {
  imode_t action = 0;
  for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
    action |= args[j].action;
  }

  o->action |= action;
  return action;
}

static imode_t get_options2(poptions_t o, const args_t args[], const char *argv) {
  imode_t action = 0;
  for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
    if (args[j].option2 && 0 == strcmp(argv, args[j].option2)) {
      action |= args[j].action;
    }
  }

  return action;
}

static imode_t get_ocdump(poptions_t o, const imodeswap_t args[], imode_t action) {
  imode_t ocdump = 0;
  for (int j = 0; (0 != args[j].mode1) || (0 != args[j].mode2); ++j) {
    if (MODE_ISSET(args[j].mode1, action)) {
      ocdump |= args[j].mode2;
    }
  }

  return ocdump;
}

static int breakup_args(char* args, char* dst0, const size_t dst0size, char* dst1, const size_t dst1size) {
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

int get_options_convert(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    usage0(o, "convert-ng", zCONVERTARGS);
    return -1;
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == strcmp(arg0, "--output")) {
          strncpy(o->outname, arg1, NELEMENTS(o->outname));
        }
      } else {
        o->action |= get_options2(o, zCONVERTARGS, argv[i]);
      }
    } else if ('-' == argv[i][0] && 0 != argv[i][1]) {
      if (0 == strcmp(argv[i], "-O")) {
        strncpy(o->outname, argv[++i], NELEMENTS(o->outname));
      } else {
        o->action |= get_options1(o, zCONVERTARGS, argv[i]);
      }
    } else if (ECODE_ISEVIL(sinsert(o, argv[i]))) {
      strncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, "convert-ng", zCONVERTARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage0(o, "convert-ng", zCONVERTARGS);
  }

  return ECODE_OK;
}

int get_options_readelf(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    usage2(o, "readelf-ng", zREADELFARGS, zREADELFARGS0, zREADELFARGS1, zREADELFARGS2, zREADELFARGS3);
    return -1;
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == strcmp(arg0, zREADELFARGS1)) {
          o->ocdump |= get_options2(o, zDEBUGELFARGS, arg1);
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
        }
      } else {
        o->action |= get_options2(o, zREADELFARGS, argv[i]);
      }
    } else if ('-' == argv[i][0]) {
      if (0 == strcmp(argv[i], zREADELFARGS0)) {
        imode_t ocdump = get_options1(o, zDEBUGELFARGS, argv[i] + 1);
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
        o->action |= get_options1(o, zREADELFARGS, argv[i]);
      }
    } else {
      strncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, "readelf-ng", zREADELFARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage2(o, "readelf-ng", zREADELFARGS, zREADELFARGS0, zREADELFARGS1, zREADELFARGS2, zREADELFARGS3);
  }

  return ECODE_OK;
}

int get_options_objcopy(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 1) {
    return -1;
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      if (0 == strcmp(argv[i], "--add-section")) {
        paction_t p = amalloc();
        if (p && ECODE_ISOK(breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname)))) {
          oinsert(o, p, ACT_ADDSECTION);
        }
      } else if (0 == strcmp(argv[i], "--dump-section")) {
        paction_t p = amalloc();
        if (p && ECODE_ISOK(breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname)))) {
          oinsert(o, p, ACT_DUMPSECTION);
        }
      } else if (0 == strcmp(argv[i], "--update-section")) {
        paction_t p = amalloc();
        if (p && ECODE_ISOK(breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname)))) {
          oinsert(o, p, ACT_UPDATESECTION);
        }
      } else {
        o->action |= get_options2(o, OBJCOPYARGS, argv[i]);
      }
    } else if ('-' == argv[i][0]) {
      o->action |= get_options1(o, OBJCOPYARGS, argv[i]);
    } else {
      strncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, "objcopy-ng", OBJCOPYARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage0(o, "objcopy-ng", OBJCOPYARGS);
  }

  return ECODE_OK;
}

int get_options_objdump(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    usage2(o, "objdump-ng", zOBJDUMPARGS, zOBJDUMPARGS0, zOBJDUMPARGS1, zOBJDUMPARGS2, zOBJDUMPARGS3);
    return ECODE_ARGUMENTS;
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 256);
      MALLOCA(char, arg1, 256);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == strcmp(arg0, zOBJDUMPARGS1)) {
          o->ocdump |= get_options2(o, zDEBUGELFARGS, arg1);
        } else if (0 == strcmp(arg0, zOBJDUMPARGS3)) {
          o->ocdump |= get_options2(o, zDISASSEMBLEARGS, arg1);
        } else if (0 == strcmp(arg0, "--start-address")) {
          o->saddress = atovalue(arg1);
        } else if (0 == strcmp(arg0, "--stop-address")) {
          o->eaddress = atovalue(arg1);
        }
      } else {
        o->action |= get_options2(o, zOBJDUMPARGS, argv[i]);
      }
    } else if ('-' == argv[i][0]) {
      if (0 == strcmp(argv[i], zOBJDUMPARGS0)) {
        imode_t ocdump = get_options1(o, zDEBUGELFARGS, argv[i] + 1);
        o->ocdump |= ocdump ? ocdump : set_options1(o, zDEBUGELFARGS);
      } else if (0 == strcmp(argv[i], zOBJDUMPARGS2)) {
        o->ocdump |= get_options2(o, zDISASSEMBLEARGS, argv[++i]);
      } else {
        o->action |= get_options1(o, zOBJDUMPARGS, argv[i]);
      }
    } else {
      strncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    }
  }

  o->ocdump |= get_ocdump(o, zOBJDUMPSWAP, o->action);

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, "objdump-ng", zOBJDUMPARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage2(o, "objdump-ng", zOBJDUMPARGS, zOBJDUMPARGS0, zOBJDUMPARGS1, zOBJDUMPARGS2, zOBJDUMPARGS3);
  }

  return ECODE_OK;
}

int get_options_objdwarf(poptions_t o, int argc, char** argv, char* name) {
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

int get_options_objhash(poptions_t o, int argc, char** argv, char* name) {
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
    free(p);
    return NULL;
  }

  return p;
}

handle_t ofree(handle_t p) {
  if (isoptions(p)) {
    afree(CAST(poptions_t, p)->actions);
    free(p);
    return NULL;
  }

  return p;
}

int oinsert(handle_t o, handle_t p, const int action) {
  if (isoptions(o) && isactions(p)) {
    poptions_t o0 = CAST(poptions_t, o);
    paction_t p0 = CAST(paction_t, p);
    p0->action  = action;
    p0->actions = o0->actions;
    o0->actions = p0;
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

