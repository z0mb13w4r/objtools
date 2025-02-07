#include <string.h>

#include "buffer.h"
#include "options.h"

typedef struct args_s {
  char  option1;
  char *option2;
  int   action;
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
  {0, 0}
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
  {'x', "--all-headers",     OPTOBJDUMP_FILEHEADER | OPTOBJDUMP_PRIVATEHEADER | OPTOBJDUMP_SECTIONHEADER},
  {0, 0}
};

static const args_t OBJDUMPARGS2[] = {
  {'l', NULL,                OPTOBJDUMP_LINENUMBERS},
  {0, 0}
};

static int get_options1(poptions_t o, const args_t args[], const char *argv) {
  for (int k = 1; k < strlen(argv); ++k) {
    for (int j = 0; 0 != args[j].option1; ++j) {
      if (argv[k] == args[j].option1) {
        o->action |= args[j].action;
        return 0;
      }
    }
  }

  return -1;
}

static int get_options2(poptions_t o, const args_t args[], const char *argv) {
  for (int j = 0; 0 != args[j].option1; ++j) {
    if (0 == strcmp(argv, args[j].option2)) {
      o->action |= args[j].action;
      return 0;
    }
  }

  return -1;
}

int get_options_readelf(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 2) {
    return -1;
  }

  o->option = OPT_READELF;
  strcpy(o->prgname, name);

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

  return 0;
}

int get_options_objcopy(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 2) {
    return -1;
  }

  o->option = OPT_OBJCOPY;
  strcpy(o->prgname, name);

  for (int i = 0; i < argc; ++i) {
  }

  return 0;
}

int get_options_objdump(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 2) {
    return -1;
  }

  o->option = OPT_OBJDUMP;
  strcpy(o->prgname, name);

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

  return 0;
}

int get_options(poptions_t o, int argc, char** argv) {
  if (argc < 4) {
    return -1;
  }

  if (0 == strcmp("--readelf", argv[1])) {
    return get_options_readelf(o, argc - 2, argv + 2, argv[0]);
  } else if (0 == strcmp("--objcopy", argv[1])) {
    return get_options_objcopy(o, argc - 2, argv + 2, argv[0]);
  } else if (0 == strcmp("--objdump", argv[1])) {
    return get_options_objdump(o, argc - 2, argv + 2, argv[0]);
  }

  return -1;
}

