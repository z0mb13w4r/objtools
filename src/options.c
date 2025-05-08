#include <string.h>

#include "buffer.h"
#include "printf.h"
#include "options.h"
#include "scripts.h"
#include "objutils.h"

#include "static/usage.ci"
#include "static/convert-ng.ci"
#include "static/objdump-ng.ci"
#include "static/readelf-ng.ci"
#include "static/objdebug-ng.ci"
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
  printf_text("NAME", USE_LT | USE_EOL);
  printf_text(name, USE_LT | USE_TAB | USE_COLON);
  printf_text(desc, USE_LT | USE_SPACE | USE_EOL);
  printf_eol();

  return 0;
}

static int usage_synopsis(poptions_t o, const char* name, const args_t args0[], const char* more0,
                                                          const args_t args1[], const char* more1) {
  MALLOCA(char, buf, 1024);

  printf_text("SYNOPSIS", USE_LT | USE_EOL);
  int n = printf_text(name, USE_LT | USE_TAB);
  if (args0[0].option1 && args0[0].option2) {
    snprintf(buf, sizeof(buf), "-%c|%s", args0[0].option1, args0[0].option2);
  } else if (args0[0].option1) {
    snprintf(buf, sizeof(buf), "-%c", args0[0].option1);
  } else if (args0[0].option2) {
    snprintf(buf, sizeof(buf), "%s", args0[0].option2);
  }
  printf_text(buf, USE_LT | USE_SPACE | USE_SB | USE_EOL);

  for (int j = 1; (0 != args0[j].option1) || (0 != args0[j].option2); ++j) {
    if (args0[j].option1 && args0[j].option2) {
       snprintf(buf, sizeof(buf), "-%c|%s", args0[j].option1, args0[j].option2);
    } else if (args0[j].option1) {
      snprintf(buf, sizeof(buf), "-%c", args0[j].option1);
    } else if (args0[j].option2) {
      snprintf(buf, sizeof(buf), "%s", args0[j].option2);
    }
    printf_pack(n);
    printf_text(buf, USE_LT | USE_SPACE | USE_SB | USE_EOL);
  }

  if (more0 && args1) {
    int x = snprintf(buf, sizeof(buf), "%s|[", more0);
    for (int j = 0; (0 != args1[j].option1) || (0 != args1[j].option2); ++j) {
      x += snprintf(buf + x, sizeof(buf) - x, "%c", args1[j].option1);
    }

    x += snprintf(buf + x, sizeof(buf) - x, "]|");
    printf_pack(n);
    printf_text(buf, USE_LT | USE_SPACE | USE_SBLT | USE_EOL);
  }

  if (more1 && args1) {
    int x = snprintf(buf, sizeof(buf), "%s|[%s", more1, args1[0].option2);
    for (int j = 1; (0 != args1[j].option1) || (0 != args1[j].option2); ++j) {
      x += snprintf(buf + x, sizeof(buf) - x, ",%s", args1[j].option2);
    }

    x += snprintf(buf + x, sizeof(buf) - x, "]");
    printf_pack(n + 2);
    printf_text(buf, USE_LT | USE_SPACE | USE_SBRT | USE_EOL);
  }

  printf_eol();

  return 0;
}

static int usage_description(poptions_t o, const char* name, const args_t args[]) {
  printf_text("DESCRIPTION", USE_LT | USE_EOL);
  printf_eol();

  return 0;
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

  }

  return n;
}

static int usage_seealso(poptions_t o, const char* name, const args_t args[]) {
  printf_text("SEE ALSO", USE_LT | USE_EOL);
  printf_book(zALLSEE, USE_LT | USE_TAB | USE_EOL);
  printf_eol();

  return 0;
}

static int usage_version(poptions_t o, const char* name, const args_t args[]) {
  printf_yoke(name, " v"VERSION_VALUE, USE_LT | USE_EOL);
  printf_eol();

  return 0;
}

static int usage_copyright(poptions_t o, const char* name, const args_t args[]) {
  printf_text("COPYRIGHT", USE_LT | USE_EOL);
  printf_book(zLICENSE, USE_LT | USE_TAB | USE_EOL);
  printf_eol();

  return 0;
}

static int usage0(poptions_t o, const char* name, const args_t args[]) {
  usage_name(o, name, args, zDESCRIPTION);
  usage_synopsis(o, name, args, NULL, NULL, NULL);
  usage_description(o, name, args);
  usage_options0(o, name, args);
  usage_seealso(o, name, args);
  usage_copyright(o, name, args);

  return 0;
}

static int usage1(poptions_t o, const char* name, const args_t args0[], const char* more0, const args_t args1[], const char* more1) {
  usage_name(o, name, args0, zDESCRIPTION);
  usage_synopsis(o, name, args0, more0, args1, more1);
  usage_description(o, name, args0);
  usage_options0(o, name, args0);
  usage_options1(o, name, args1, more0, more1);
  usage_seealso(o, name, args0);
  usage_copyright(o, name, args0);

  return 0;
}

static int version0(poptions_t o, const char* name, const args_t args[]) {
  usage_version(o, name, args);
  usage_copyright(o, name, args);

  return 0;
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
      return 0;
    }
  }

  return -1;
}

int get_options_convert(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    usage0(o, "convert-ng", zCONVERTARGS);
    return -1;
  }

  o->option = OPT_CONVERT;
  strname(o->prgname, name);
  set_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (0 == breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1))) {
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
    } else if (0 != sinsert(o, argv[i])) {
      strncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, "convert-ng", zCONVERTARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage0(o, "convert-ng", zCONVERTARGS);
  }

  return 0;
}

int get_options_readelf(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    usage1(o, "readelf-ng", zREADELFARGS, zREADELFARGS0, zDEBUGELFARGS, zREADELFARGS1);
    return -1;
  }

  o->option = OPT_READELF;
  strname(o->prgname, name);
  set_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (0 == breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1))) {
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
    return usage1(o, "readelf-ng", zREADELFARGS, zREADELFARGS0, zDEBUGELFARGS, zREADELFARGS1);
  }

  return 0;
}

int get_options_objcopy(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 1) {
    return -1;
  }

  o->option = OPT_OBJCOPY;
  strname(o->prgname, name);
  set_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      if (0 == strcmp(argv[i], "--add-section")) {
        paction_t p = amalloc();
        if (p && 0 == breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname))) {
          oinsert(o, p, ACT_ADDSECTION);
        }
      } else if (0 == strcmp(argv[i], "--dump-section")) {
        paction_t p = amalloc();
        if (p && 0 == breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname))) {
          oinsert(o, p, ACT_DUMPSECTION);
        }
      } else if (0 == strcmp(argv[i], "--update-section")) {
        paction_t p = amalloc();
        if (p && 0 == breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname))) {
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

  return 0;
}

int get_options_objdump(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    usage1(o, "objdump-ng", zOBJDUMPARGS, zOBJDUMPARGS0, zDEBUGELFARGS, zOBJDUMPARGS1);
    return -1;
  }

  o->option = OPT_OBJDUMP;
  strname(o->prgname, name);
  set_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 256);
      MALLOCA(char, arg1, 256);

      if (0 == breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1))) {
        if (0 == strcmp(arg0, zOBJDUMPARGS1)) {
          o->ocdump |= get_options2(o, zDEBUGELFARGS, arg1);
        } else if (0 == strcmp(arg0, zOBJDUMPARGS3)) {
          o->ocdump |= get_options2(o, zDISASSEMBLEARGS, arg1);
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

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, "objdump-ng", zOBJDUMPARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage1(o, "objdump-ng", zOBJDUMPARGS, zOBJDUMPARGS0, zDEBUGELFARGS, zOBJDUMPARGS1);
  }

  return 0;
}

int get_options_objhash(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 1) {
    return -1;
  }

  o->option = OPT_OBJHASH;
  strname(o->prgname, name);
  set_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 256);
      MALLOCA(char, arg1, 256);

      if (0 == breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1))) {
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

  return 0;
}

int get_options(poptions_t o, int argc, char** argv) {
  if (argc < 3) {
    return -1;
  }

  if (0 == strcmp("--readelf", argv[1])) {
    return get_options_readelf(o, argc - 2, argv + 2, argv[0]);
  } else if (0 == strcmp("--objcopy", argv[1])) {
    return get_options_objcopy(o, argc - 2, argv + 2, argv[0]);
  } else if (0 == strcmp("--objdump", argv[1])) {
    return get_options_objdump(o, argc - 2, argv + 2, argv[0]);
  } else if (0 == strcmp("--objhash", argv[1])) {
    return get_options_objhash(o, argc - 2, argv + 2, argv[0]);
  }

  return -1;
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
    p->saddress = CAST(uint64_t, -1); /* --start-address */
    p->eaddress = CAST(uint64_t, -1); /* --stop-address */
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
    return 0;
  }

  return -1;
}

int oinsertvalue(handle_t o, const int action, const uint64_t value) {
  if (isoptions(o)) {
    paction_t p = amalloc();
    if (isactions(p)) {
      p->value = value;
      oinsert(o, p, action);
      return 0;
    }
  }

  return -1;
}

int oinsertsecname(handle_t o, const int action, const char *secname) {
  if (isoptions(o)) {
    paction_t p = amalloc();
    if (isactions(p)) {
      strncpy(p->secname, secname, NELEMENTS(p->secname));
      oinsert(o, p, action);
      return 0;
    }
  }

  return -1;
}

