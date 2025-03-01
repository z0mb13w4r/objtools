#include <string.h>

#include "buffer.h"
#include "printf.h"
#include "options.h"
#include "objutils.h"

#include "static/usage.ci"

typedef struct args_s {
  char    option1;
  char   *option2;
  imode_t action;
} args_t, *pargs_t;

static const args_t READELFARGS[] = {
  {'A', "--arch-specific",   OPTREADELF_ARCH},
  {'I', "--histogram",       OPTREADELF_HISTOGRAM},
  {'S', "--section-headers", OPTREADELF_SECTIONHEADERS},
  {'V', NULL,                OPTREADELF_VERSION},
  {'a', "--all",             OPTREADELF_ALL},
  {'d', "--dynamic",         OPTREADELF_DYNAMIC},
  {'e', "--headers",         OPTREADELF_FILEHEADER | OPTREADELF_SECTIONHEADERS | OPTREADELF_PROGRAMHEADERS},
  {'g', "--section-groups",  OPTREADELF_SECTIONGROUPS},
  {'h', "--file-header",     OPTREADELF_FILEHEADER},
  {'l', "--program-headers", OPTREADELF_PROGRAMHEADERS},
  {'n', "--notes",           OPTREADELF_NOTES},
  {'r', "--relocs",          OPTREADELF_RELOCS},
  {'s', "--symbols",         OPTREADELF_SYMBOLS},
  {'u', "--unwind",          OPTREADELF_UNWIND},
  {'H', "--help",            OPTPROGRAM_HELP},
  {0, NULL}
};

static const args_t OBJCOPYARGS[] = {
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
  {'H', "--help",                           OPTPROGRAM_HELP},
  {0, NULL},
};

static const args_t OBJDUMPARGS1[] = {
  {'D', "--disassemble-all", OPTOBJDUMP_DISADDEMBLEALL | OPTOBJDUMP_DISASSEMBLE},
  {'S', "--source",          OPTOBJDUMP_SOURCECODE | OPTOBJDUMP_DISASSEMBLE},
  {'T', "--dynamic-symbols", OPTOBJDUMP_DYNAMICSYMBOLS},
  {'d', "--disassemble",     OPTOBJDUMP_DISASSEMBLE},
  {'g', "--debugging",       OPTOBJDUMP_DEBUGGING},
  {'s', "--full-contents",   OPTOBJDUMP_SECTIONS},
  {'t', "--symbols",         OPTOBJDUMP_SYMBOLS},
  {'f', "--file-headers",    OPTOBJDUMP_FILEHEADER},
  {'p', "--private-headers", OPTOBJDUMP_PRIVATEHEADER},
  {'h', "--section-headers", OPTOBJDUMP_SECTIONHEADER},
  {'x', "--all-headers",     OPTOBJDUMP_FILEHEADER | OPTOBJDUMP_PRIVATEHEADER | OPTOBJDUMP_SECTIONHEADER | OPTOBJDUMP_SYMBOLS},
  {'H', "--help",            OPTPROGRAM_HELP},
  {0, NULL}
};

static const args_t OBJDUMPARGS2[] = {
  {'l', NULL,                OPTOBJDUMP_LINENUMBERS},
  {0, NULL}
};

static int usage_name(poptions_t o, const char* name, const args_t args[]) {
  printf_text("NAME", USE_LT | USE_EOL);
  printf_text(name, USE_LT | USE_TAB | USE_COLON | USE_EOL);
  printf_eol();

  return 0;
}

static int usage_synopsis(poptions_t o, const char* name, const args_t args[]) {
  MALLOCA(char, buf, 1024);

  printf_text("SYNOPSIS", USE_LT | USE_EOL);
  int n = printf_text(name, USE_LT | USE_TAB);
  if (args[0].option1 && args[0].option2) {
    snprintf(buf, sizeof(buf), "-%c|%s", args[0].option1, args[0].option2);
  } else if (args[0].option1) {
    snprintf(buf, sizeof(buf), "-%c", args[0].option1);
  } else if (args[0].option2) {
    snprintf(buf, sizeof(buf), "%s", args[0].option2);
  }
  printf_text(buf, USE_LT | USE_SPACE | USE_SB | USE_EOL);

  for (int j = 1; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
    if (args[j].option1 && args[j].option2) {
       snprintf(buf, sizeof(buf), "-%c|%s", args[j].option1, args[j].option2);
    } else if (args[j].option1) {
      snprintf(buf, sizeof(buf), "-%c", args[j].option1);
    } else if (args[j].option2) {
      snprintf(buf, sizeof(buf), "%s", args[j].option2);
    }
    printf_pack(n);
    printf_text(buf, USE_LT | USE_SPACE | USE_SB | USE_EOL);
  }
  printf_eol();

  return 0;
}

static int usage_description(poptions_t o, const char* name, const args_t args[]) {
  printf_text("DESCRIPTION", USE_LT | USE_EOL);
  printf_eol();

  return 0;
}

static int usage_options(poptions_t o, const char* name, const args_t args[]) {
  printf_text("OPTIONS", USE_LT | USE_EOL);
  for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
    if (args[j].option1) {
      printf_nice(args[j].option1, USE_CHAR | USE_TAB | USE_DASH | USE_EOL);
    }
    if (args[j].option2) {
      printf_text(args[j].option2, USE_LT | USE_TAB | USE_EOL);
    }
    if (isbits(args[j].action)) {
      printf_pack(4);
      printf_text("Equivalent to specifying", USE_LT | USE_COLON);
      for (int k = 0; (0 != args[k].option1) || (0 != args[k].option2); ++k) {
        if (k != j && !isbits(args[k].action) && (args[k].action & args[j].action)) {
          printf_text(args[k].option2, USE_LT | USE_SPACE);
        }
      }
      printf_eol();
    }
    printf_eol();
  }

  return 0;
}

static int usage_seealso(poptions_t o, const char* name, const args_t args[]) {
  printf_text("SEE ALSO", USE_LT | USE_EOL);
  printf_book(ALLSEE, USE_LT | USE_TAB | USE_EOL);
  printf_eol();

  return 0;
}

static int usage_copyright(poptions_t o, const char* name, const args_t args[]) {
  printf_text("COPYRIGHT", USE_LT | USE_EOL);
  printf_book(LICENSE, USE_LT | USE_TAB | USE_EOL);
  printf_eol();

  return 0;
}

static int usage(poptions_t o, const char* name, const args_t args[]) {
  usage_name(o, name, args);
  usage_synopsis(o, name, args);
  usage_description(o, name, args);
  usage_options(o, name, args);
  usage_seealso(o, name, args);
  usage_copyright(o, name, args);

  return 1;
}

static int get_options1(poptions_t o, const args_t args[], const char *argv) {
  for (int k = 1; k < strlen(argv); ++k) {
    for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
      if (argv[k] == args[j].option1) {
        o->action |= args[j].action;
        break;
      }
    }
  }

  return 0;
}

static int get_options2(poptions_t o, const args_t args[], const char *argv) {
  for (int j = 0; (0 != args[j].option1) || (0 != args[j].option2); ++j) {
    if (0 == strcmp(argv, args[j].option2)) {
      o->action |= args[j].action;
    }
  }

  return -1;
}

static int breakup_args(paction_t p, char *src) {
  MALLOCA(char, buf, 1024);
  strncpy(buf, src, NELEMENTS(buf));

  const char DELIMITS[] = "=";
  char* token = strtok(buf, DELIMITS);
  if (token) {
    strncpy(p->secname, token, NELEMENTS(p->secname));
    token = strtok(NULL, DELIMITS);
    if (token) {
      strncpy(p->outname, token, NELEMENTS(p->outname));
      return 0;
    }
  }
  return -1;
}

int get_options_readelf(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 1) {
    return -1;
  }

  o->option = OPT_READELF;
  strname(o->prgname, name);
  set_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      if (0 == strncmp(argv[i], "--hex-dump=", 11)) {
        paction_t p = create(MODE_ACTIONS);
        strcpy(p->secname, argv[i] + 11);
        p->action = ACT_HEXDUMP;
        p->actions = o->actions;
        o->actions = p;
      } else if (0 == strncmp(argv[i], "--string-dump=", 14)) {
        paction_t p = create(MODE_ACTIONS);
        strcpy(p->secname, argv[i] + 14);
        p->action = ACT_STRDUMP;
        p->actions = o->actions;
        o->actions = p;
      } else if (0 == strncmp(argv[i], "--relocated-dump=", 17)) {
        paction_t p = create(MODE_ACTIONS);
        strcpy(p->secname, argv[i] + 17);
        p->action = ACT_RELDUMP;
        p->actions = o->actions;
        o->actions = p;
      } else {
        get_options2(o, READELFARGS, argv[i]);
      }
    } else if ('-' == argv[i][0]) {
      if (0 == strcmp(argv[i], "-x")) {
        paction_t p = create(MODE_ACTIONS);
	strcpy(p->secname, argv[++i]);
        p->action = ACT_HEXDUMP;
        p->actions = o->actions;
        o->actions = p;
      } else if (0 == strcmp(argv[i], "-p")) {
        paction_t p = create(MODE_ACTIONS);
        strcpy(p->secname, argv[++i]);
        p->action = ACT_STRDUMP;
        p->actions = o->actions;
        o->actions = p;
      } else if (0 == strcmp(argv[i], "-R")) {
        paction_t p = create(MODE_ACTIONS);
        strcpy(p->secname, argv[++i]);
        p->action = ACT_RELDUMP;
        p->actions = o->actions;
        o->actions = p;
      } else {
        get_options1(o, READELFARGS, argv[i]);
      }
    } else {
      strcpy(o->inpname, argv[i]);
    }
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage(o, "readelf-ng", READELFARGS);
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
        paction_t p = create(MODE_ACTIONS);
        if (0 == breakup_args(p, argv[++i])) {
          p->action = ACT_ADDSECTION;
          p->actions = o->actions;
          o->actions = p;
        }
      } else if (0 == strcmp(argv[i], "--dump-section")) {
        paction_t p = create(MODE_ACTIONS);
        if (0 == breakup_args(p, argv[++i])) {
          p->action = ACT_DUMPSECTION;
          p->actions = o->actions;
          o->actions = p;
        }
      } else if (0 == strcmp(argv[i], "--update-section")) {
        paction_t p = create(MODE_ACTIONS);
        if (0 == breakup_args(p, argv[++i])) {
          p->action = ACT_UPDATESECTION;
          p->actions = o->actions;
          o->actions = p;
        }
      } else {
        get_options2(o, OBJCOPYARGS, argv[i]);
      }
    } else if ('-' == argv[i][0]) {
      get_options1(o, OBJCOPYARGS, argv[i]);
    } else {
      strcpy(o->inpname, argv[i]);
    }
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage(o, "objcopy-ng", OBJCOPYARGS);
  }

  return 0;
}

int get_options_objdump(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 1) {
    return -1;
  }

  o->option = OPT_OBJDUMP;
  strname(o->prgname, name);
  set_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      get_options2(o, OBJDUMPARGS1, argv[i]);
    } else if ('-' == argv[i][0]) {
      if (0 == strcmp(argv[i], "-W")) {
        get_options1(o, OBJDUMPARGS2, argv[i] + 1);
      } else {
        get_options1(o, OBJDUMPARGS1, argv[i]);
      }
    } else {
      strcpy(o->inpname, argv[i]);
    }
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage(o, "objdump-ng", OBJDUMPARGS1);
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

    } else if ('-' == argv[i][0]) {

    } else if (0 == o->inpname0[0]) {
      strcpy(o->inpname0, argv[i]);
    } else if (0 == o->inpname1[0]) {
      strcpy(o->inpname1, argv[i]);
    }
  }

//  if (o->action & OPTPROGRAM_HELP) {
//    return usage(o, "objhash-ng", READELFARGS);
//  }

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

