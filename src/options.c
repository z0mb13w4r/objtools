#include <stdio.h>
#include <string.h>

#include "buffer.h"
#include "options.h"

static int get_options_readelf(poptions_t o, int argc, char** argv) {
  o->option = OPT_READELF;

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0]) {
      if (0 == strcmp(argv[i], "-a") || 0 == strcmp(argv[i], "--all")) {
        o->action |= OPTREADELF_SYMBOLS;
        o->action |= OPTREADELF_RELOCS;
        o->action |= OPTREADELF_UNWIND;
        o->action |= OPTREADELF_DYNAMIC;
        o->action |= OPTREADELF_FILEHEADER;
        o->action |= OPTREADELF_SECTIONHEADERS;
        o->action |= OPTREADELF_SECTIONGROUPS;
        o->action |= OPTREADELF_PROGRAMHEADERS;
        o->action |= OPTREADELF_VERSION;
        o->action |= OPTREADELF_HISTOGRAM;
        o->action |= OPTREADELF_ARCH;
        o->action |= OPTREADELF_NOTES;
      } else if (0 == strcmp(argv[i], "-g") || 0 == strcmp(argv[i], "--section-groups")) {
        o->action |= OPTREADELF_SECTIONGROUPS;
      } else if (0 == strcmp(argv[i], "-e") || 0 == strcmp(argv[i], "--headers")) {
        o->action |= OPTREADELF_FILEHEADER;
        o->action |= OPTREADELF_SECTIONHEADERS;
        o->action |= OPTREADELF_PROGRAMHEADERS;
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

int get_options(poptions_t o, int argc, char** argv) {
  if (argc < 4) {
    return -1;
  }

  if (0 == strcmp("--readelf", argv[1])) {
    return get_options_readelf(o, argc - 2, argv + 2);
  }

  return -1;
}

