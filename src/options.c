#include <stdio.h>
#include <string.h>

#include "options.h"

static int get_options_readelf(poptions_t o, int argc, char** argv) {
  memset(o, 0, sizeof(options_t));
  o->option = OPT_READELF;

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0]) {
      if (0 == strcmp(argv[i], "-a") || 0 == strcmp(argv[i], "--all")) {
        o->readelf.do_symbols = -1;
        o->readelf.do_reloc = -1;
        o->readelf.do_unwind = -1;
        o->readelf.do_dynamic = -1;
        o->readelf.do_header = -1;
        o->readelf.do_sections = -1;
        o->readelf.do_section_groups = -1;
        o->readelf.do_segments = -1;
        o->readelf.do_version = -1;
        o->readelf.do_histogram = -1;
        o->readelf.do_arch = -1;
        o->readelf.do_notes = -1;
      } if (0 == strcmp(argv[i], "-g") || 0 == strcmp(argv[i], "--section-groups")) {
        o->readelf.do_section_groups = -1;
      } if (0 == strcmp(argv[i], "-e") || 0 == strcmp(argv[i], "--headers")) {
        o->readelf.do_header = -1;
        o->readelf.do_sections = -1;
        o->readelf.do_segments = -1;
      } if (0 == strcmp(argv[i], "-A") || 0 == strcmp(argv[i], "--arch-specific")) {
        o->readelf.do_arch = -1;
      } if (0 == strcmp(argv[i], "-r") || 0 == strcmp(argv[i], "--relocs")) {
        o->readelf.do_reloc = -1;
      } if (0 == strcmp(argv[i], "-u") || 0 == strcmp(argv[i], "--unwind")) {
        o->readelf.do_unwind = -1;
      } if (0 == strcmp(argv[i], "-h") || 0 == strcmp(argv[i], "--file-header")) {
        o->readelf.do_header = -1;
      } if (0 == strcmp(argv[i], "-l") || 0 == strcmp(argv[i], "--program-headers")) {
        o->readelf.do_segments = -1;
      } if (0 == strcmp(argv[i], "-s") || 0 == strcmp(argv[i], "--symbols")) {
        o->readelf.do_symbols = -1;
      } if (0 == strcmp(argv[i], "-S") || 0 == strcmp(argv[i], "--section-headers")) {
        o->readelf.do_sections = -1;
      } if (0 == strcmp(argv[i], "-d") || 0 == strcmp(argv[i], "--dynamic")) {
        o->readelf.do_dynamic = -1;
      } if (0 == strcmp(argv[i], "-I") || 0 == strcmp(argv[i], "--histogram")) {
        o->readelf.do_histogram = -1;
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

