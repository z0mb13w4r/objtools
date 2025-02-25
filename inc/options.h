#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#include <limits.h>

#define OPT_READELF                                (0)
#define OPT_OBJCOPY                                (1)
#define OPT_OBJDUMP                                (2)
#define OPT_OBJHASH                                (3)

#define OPTPROGRAM_HELP                            (1ULL << 48)

#define OPTREADELF_FILEHEADER                      (1 << 0)
#define OPTREADELF_SECTIONGROUPS                   (1 << 1)
#define OPTREADELF_SECTIONHEADERS                  (1 << 2)
#define OPTREADELF_PROGRAMHEADERS                  (1 << 3)
#define OPTREADELF_SYMBOLS                         (1 << 4)
#define OPTREADELF_RELOCS                          (1 << 5)
#define OPTREADELF_UNWIND                          (1 << 6)
#define OPTREADELF_DYNAMIC                         (1 << 7)
#define OPTREADELF_HISTOGRAM                       (1 << 8)
#define OPTREADELF_ARCH                            (1 << 9)
#define OPTREADELF_NOTES                           (1 << 10)
#define OPTREADELF_VERSION                         (1 << 11)

#define OPTREADELF_ALL                 (OPTREADELF_SYMBOLS | OPTREADELF_RELOCS | OPTREADELF_UNWIND \
                                           | OPTREADELF_DYNAMIC | OPTREADELF_FILEHEADER | OPTREADELF_SECTIONHEADERS \
                                           | OPTREADELF_SECTIONGROUPS | OPTREADELF_PROGRAMHEADERS | OPTREADELF_VERSION \
                                           | OPTREADELF_HISTOGRAM | OPTREADELF_ARCH | OPTREADELF_NOTES)


#define OPTOBJCOPY_CHANGE_WARNINGS                 (1 << 0)
#define OPTOBJCOPY_DEBUGGING                       (1 << 1)
#define OPTOBJCOPY_DECOMPRESS_DEBUG_SECTIONS       (1 << 2)
#define OPTOBJCOPY_DISABLE_DETERMINISTIC_ARCHIVES  (1 << 3)
#define OPTOBJCOPY_DISCARD_ALL                     (1 << 4)
#define OPTOBJCOPY_DISCARD_LOCALS                  (1 << 5)
#define OPTOBJCOPY_ENABLE_DETERMINISTIC_ARCHIVES   (1 << 6)
#define OPTOBJCOPY_EXTRACT_DWO                     (1 << 7)
#define OPTOBJCOPY_EXTRACT_SYMBOL                  (1 << 8)
#define OPTOBJCOPY_IMPURE                          (1 << 9)
#define OPTOBJCOPY_KEEP_FILE_SYMBOLS               (1 << 10)
#define OPTOBJCOPY_LOCALIZE_HIDDEN                 (1 << 11)
#define OPTOBJCOPY_MERGE_NOTES                     (1 << 12)
#define OPTOBJCOPY_NO_CHANGE_WARNINGS              (1 << 13)
#define OPTOBJCOPY_NO_MERGE_NOTES                  (1 << 14)
#define OPTOBJCOPY_ONLY_KEEP_DEBUG                 (1 << 15)
#define OPTOBJCOPY_PRESERVE_DATES                  (1 << 16)
#define OPTOBJCOPY_PURE                            (1 << 17)
#define OPTOBJCOPY_READONLY_TEXT                   (1 << 18)
#define OPTOBJCOPY_STRIP_ALL                       (1 << 19)
#define OPTOBJCOPY_STRIP_DEBUG                     (1 << 20)
#define OPTOBJCOPY_STRIP_DWO                       (1 << 21)
#define OPTOBJCOPY_STRIP_SECTION_HEADERS           (1 << 22)
#define OPTOBJCOPY_STRIP_UNNEEDED                  (1 << 23)
#define OPTOBJCOPY_VERBOSE                         (1 << 24)
#define OPTOBJCOPY_WEAKEN                          (1 << 25)
#define OPTOBJCOPY_WRITABLE_TEXT                   (1 << 26)
#define OPTOBJCOPY_DUMP_SECTIONS_ALL               (1 << 27)

#define OPTOBJDUMP_DISADDEMBLEALL                  (1 << 0)
#define OPTOBJDUMP_SOURCECODE                      (1 << 1)
#define OPTOBJDUMP_DYNAMICSYMBOLS                  (1 << 2)
#define OPTOBJDUMP_DISASSEMBLE                     (1 << 3)
#define OPTOBJDUMP_DEBUGGING                       (1 << 4)
#define OPTOBJDUMP_SECTIONS                        (1 << 5)
#define OPTOBJDUMP_SYMBOLS                         (1 << 6)
#define OPTOBJDUMP_LINENUMBERS                     (1 << 7)
#define OPTOBJDUMP_FILEHEADER                      (1 << 8)
#define OPTOBJDUMP_PRIVATEHEADER                   (1 << 9)
#define OPTOBJDUMP_SECTIONHEADER                   (1 << 10)

#define ACT_HEXDUMP                                (0)
#define ACT_STRDUMP                                (1)
#define ACT_RELDUMP                                (2)
#define ACT_ADDSECTION                             (3)
#define ACT_DUMPSECTION                            (4)
#define ACT_UPDATESECTION                          (5)

typedef struct action_s {
  smode_t mode;
  int     action;
  char    secname[256];
  char    outname[PATH_MAX];
  struct action_s *actions;
} action_t, *paction_t;

typedef struct options_s {
  smode_t mode;
  imode_t option;
  imode_t action;

  union {
    char inpname[PATH_MAX];
    char inpname0[PATH_MAX];
  };
  union {
    char outname[PATH_MAX];
    char inpname1[PATH_MAX];
  };
  char prgname[256];

  paction_t actions;
} options_t, *poptions_t;

int get_options_readelf(poptions_t o, int argc, char** argv, char* name);
int get_options_objcopy(poptions_t o, int argc, char** argv, char* name);
int get_options_objdump(poptions_t o, int argc, char** argv, char* name);
int get_options_objhash(poptions_t o, int argc, char** argv, char* name);

int get_options(poptions_t o, int argc, char** argv);

#endif

