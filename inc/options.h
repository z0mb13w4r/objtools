#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#define OPT_READELF          (0)

typedef struct readelf_s {
  int do_symbols;
  int do_reloc;
  int do_unwind;
  int do_dynamic;
  int do_header;
  int do_sections;
  int do_section_groups;
  int do_segments;
  int do_version;
  int do_histogram;
  int do_arch;
  int do_notes;
} readelf_t, *preadelf;

typedef struct options_s {
  int option;
  char inpname[1024];
  char outname[1024];
  union {
    readelf_t readelf;
  };
} options_t, *poptions_t;

int get_options(poptions_t o, int argc, char** argv);

#endif

