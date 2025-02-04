#include <string.h>

#include "buffer.h"
#include "options.h"

typedef struct args_s {
  char  option1;
  char *option2;
  int   actions;
} args_t, *pargs_t;

static const args_t READELFARGS[] = {
  {'a', "--all", OPTREADELF_ALL},
  {'g', "--section-groups",  OPTREADELF_SECTIONGROUPS},
  {'e', "--headers",         OPTREADELF_FILEHEADER | OPTREADELF_SECTIONHEADERS | OPTREADELF_PROGRAMHEADERS},
  {'A', "--arch-specific",   OPTREADELF_ARCH},
  {'r', "--relocs",          OPTREADELF_RELOCS},
  {'u', "--unwind",          OPTREADELF_UNWIND},
  {'h', "--file-header",     OPTREADELF_FILEHEADER},
  {'l', "--program-headers", OPTREADELF_PROGRAMHEADERS},
  {'s', "--symbols",         OPTREADELF_SYMBOLS},
  {'S', "--section-headers", OPTREADELF_SECTIONHEADERS},
  {'d', "--dynamic",         OPTREADELF_DYNAMIC},
  {'I', "--histogram",       OPTREADELF_HISTOGRAM},
  {'n', "--notes",           OPTREADELF_NOTES},
  {'V', NULL,                OPTREADELF_VERSION},
  {0, 0}
};

int get_options_readelf(poptions_t o, int argc, char** argv) {
  if (argc < 2) {
    return -1;
  }

  o->option = OPT_READELF;

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0]) {
      if (0 == strcmp(argv[i], "-a") || 0 == strcmp(argv[i], "--all")) {
        o->action |= OPTREADELF_ALL;
      } else if (0 == strcmp(argv[i], "-g") || 0 == strcmp(argv[i], "--section-groups")) {
        o->action |= OPTREADELF_SECTIONGROUPS;
      } else if (0 == strcmp(argv[i], "-e") || 0 == strcmp(argv[i], "--headers")) {
        o->action |= OPTREADELF_FILEHEADER | OPTREADELF_SECTIONHEADERS | OPTREADELF_PROGRAMHEADERS;
      } else if (0 == strcmp(argv[i], "-A") || 0 == strcmp(argv[i], "--arch-specific")) {
        o->action |= OPTREADELF_ARCH;
      } else if (0 == strcmp(argv[i], "-r") || 0 == strcmp(argv[i], "--relocs")) {
        o->action |= OPTREADELF_RELOCS;
      } else if (0 == strcmp(argv[i], "-u") || 0 == strcmp(argv[i], "--unwind")) {
        o->action |= OPTREADELF_UNWIND;
      } else if (0 == strcmp(argv[i], "-h") || 0 == strcmp(argv[i], "--file-header")) {
        o->action |= OPTREADELF_FILEHEADER;
      } else if (0 == strcmp(argv[i], "-l") || 0 == strcmp(argv[i], "--program-headers")) {
        o->action |= OPTREADELF_PROGRAMHEADERS;
      } else if (0 == strcmp(argv[i], "-s") || 0 == strcmp(argv[i], "--symbols")) {
        o->action |= OPTREADELF_SYMBOLS;
      } else if (0 == strcmp(argv[i], "-S") || 0 == strcmp(argv[i], "--section-headers")) {
        o->action |= OPTREADELF_SECTIONHEADERS;
      } else if (0 == strcmp(argv[i], "-d") || 0 == strcmp(argv[i], "--dynamic")) {
        o->action |= OPTREADELF_DYNAMIC;
      } else if (0 == strcmp(argv[i], "-I") || 0 == strcmp(argv[i], "--histogram")) {
        o->action |= OPTREADELF_HISTOGRAM;
      } else if (0 == strcmp(argv[i], "-n") || 0 == strcmp(argv[i], "--notes")) {
        o->action |= OPTREADELF_NOTES;
      } else if (0 == strcmp(argv[i], "-V")) {
        o->action |= OPTREADELF_VERSION;
      } else if (0 == strcmp(argv[i], "-x")) {
        paction_t p = create(MODE_ACTIONS);
	strcpy(p->secname, argv[++i]);
	p->action = ACT_HEXDUMP;
	p->actions = o->actions;
	o->actions = p;
      } else if (0 == strncmp(argv[i], "--hex-dump=", 11)) {
        paction_t p = create(MODE_ACTIONS);
        strcpy(p->secname, argv[i] + 11);
        p->action = ACT_HEXDUMP;
        p->actions = o->actions;
        o->actions = p;
      } else if (0 == strcmp(argv[i], "-p")) {
        paction_t p = create(MODE_ACTIONS);
	strcpy(p->secname, argv[++i]);
	p->action = ACT_STRDUMP;
	p->actions = o->actions;
	o->actions = p;
      } else if (0 == strncmp(argv[i], "--string-dump=", 14)) {
        paction_t p = create(MODE_ACTIONS);
        strcpy(p->secname, argv[i] + 14);
        p->action = ACT_STRDUMP;
        p->actions = o->actions;
        o->actions = p;
      }
    } else {
      strcpy(o->inpname, argv[i]);
    }
  }

  return 0;
}

int get_options_objcopy(poptions_t o, int argc, char** argv) {
  if (argc < 3) {
    return -1;
  }

  o->option = OPT_OBJCOPY;

  for (int i = 0; i < argc; ++i) {
  }

  return 0;
}

int get_options_objdump(poptions_t o, int argc, char** argv) {
  if (argc < 3) {
    return -1;
  }

  o->option = OPT_OBJDUMP;

  for (int i = 0; i < argc; ++i) {
  }

  return 0;
}

int get_options(poptions_t o, int argc, char** argv) {
  if (argc < 4) {
    return -1;
  }

  if (0 == strcmp("--readelf", argv[1])) {
    return get_options_readelf(o, argc - 2, argv + 2);
  } else if (0 == strcmp("--objcopy", argv[1])) {
    return get_options_objcopy(o, argc - 2, argv + 2);
  } else if (0 == strcmp("--objdump", argv[1])) {
    return get_options_objdump(o, argc - 2, argv + 2);
  }

  return -1;
}

