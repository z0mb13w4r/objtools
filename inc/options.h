#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#include "defines.h"

#define OPT_READELF                                (0)
#define OPT_OBJCOPY                                (1)
#define OPT_OBJDUMP                                (2)
#define OPT_OBJHASH                                (3)

#define OPTPROGRAM_HELP                            U64MASK(48)

#define OPTREADELF_FILEHEADER                      U64MASK(0)
#define OPTREADELF_SECTIONGROUPS                   U64MASK(1)
#define OPTREADELF_SECTIONHEADERS                  U64MASK(2)
#define OPTREADELF_PROGRAMHEADERS                  U64MASK(3)
#define OPTREADELF_SYMBOLS                         U64MASK(4)
#define OPTREADELF_RELOCS                          U64MASK(5)
#define OPTREADELF_UNWIND                          U64MASK(6)
#define OPTREADELF_DYNAMIC                         U64MASK(7)
#define OPTREADELF_HISTOGRAM                       U64MASK(8)
#define OPTREADELF_ARCH                            U64MASK(9)
#define OPTREADELF_NOTES                           U64MASK(10)
#define OPTREADELF_VERSION                         U64MASK(11)

#define OPTREADELF_ALL                 (OPTREADELF_SYMBOLS | OPTREADELF_RELOCS | OPTREADELF_UNWIND \
                                           | OPTREADELF_DYNAMIC | OPTREADELF_FILEHEADER | OPTREADELF_SECTIONHEADERS \
                                           | OPTREADELF_SECTIONGROUPS | OPTREADELF_PROGRAMHEADERS | OPTREADELF_VERSION \
                                           | OPTREADELF_HISTOGRAM | OPTREADELF_ARCH | OPTREADELF_NOTES)


#define OPTOBJCOPY_CHANGE_WARNINGS                 U64MASK(0)
#define OPTOBJCOPY_DEBUGGING                       U64MASK(1)
#define OPTOBJCOPY_DECOMPRESS_DEBUG_SECTIONS       U64MASK(2)
#define OPTOBJCOPY_DISABLE_DETERMINISTIC_ARCHIVES  U64MASK(3)
#define OPTOBJCOPY_DISCARD_ALL                     U64MASK(4)
#define OPTOBJCOPY_DISCARD_LOCALS                  U64MASK(5)
#define OPTOBJCOPY_ENABLE_DETERMINISTIC_ARCHIVES   U64MASK(6)
#define OPTOBJCOPY_EXTRACT_DWO                     U64MASK(7)
#define OPTOBJCOPY_EXTRACT_SYMBOL                  U64MASK(8)
#define OPTOBJCOPY_IMPURE                          U64MASK(9)
#define OPTOBJCOPY_KEEP_FILE_SYMBOLS               U64MASK(10)
#define OPTOBJCOPY_LOCALIZE_HIDDEN                 U64MASK(11)
#define OPTOBJCOPY_MERGE_NOTES                     U64MASK(12)
#define OPTOBJCOPY_NO_CHANGE_WARNINGS              U64MASK(13)
#define OPTOBJCOPY_NO_MERGE_NOTES                  U64MASK(14)
#define OPTOBJCOPY_ONLY_KEEP_DEBUG                 U64MASK(15)
#define OPTOBJCOPY_PRESERVE_DATES                  U64MASK(16)
#define OPTOBJCOPY_PURE                            U64MASK(17)
#define OPTOBJCOPY_READONLY_TEXT                   U64MASK(18)
#define OPTOBJCOPY_STRIP_ALL                       U64MASK(19)
#define OPTOBJCOPY_STRIP_DEBUG                     U64MASK(20)
#define OPTOBJCOPY_STRIP_DWO                       U64MASK(21)
#define OPTOBJCOPY_STRIP_SECTION_HEADERS           U64MASK(22)
#define OPTOBJCOPY_STRIP_UNNEEDED                  U64MASK(23)
#define OPTOBJCOPY_VERBOSE                         U64MASK(24)
#define OPTOBJCOPY_WEAKEN                          U64MASK(25)
#define OPTOBJCOPY_WRITABLE_TEXT                   U64MASK(26)
#define OPTOBJCOPY_DUMP_SECTIONS_ALL               U64MASK(27)

#define OPTOBJDUMP_DISASSEMBLE_ALL                 U64MASK(0)
#define OPTOBJDUMP_DISASSEMBLE                     U64MASK(1)
#define OPTOBJDUMP_SOURCE_CODE                     U64MASK(2)
#define OPTOBJDUMP_DYNAMIC_SYMBOLS                 U64MASK(3)
#define OPTOBJDUMP_DEBUGGING                       U64MASK(4)
#define OPTOBJDUMP_DEBUGGING_TAGS                  U64MASK(5)
#define OPTOBJDUMP_SECTIONS                        U64MASK(6)
#define OPTOBJDUMP_SYMBOLS                         U64MASK(7)
#define OPTOBJDUMP_LINE_NUMBERS                    U64MASK(8)
#define OPTOBJDUMP_FILE_HEADER                     U64MASK(9)
#define OPTOBJDUMP_PRIVATE_HEADER                  U64MASK(10)
#define OPTOBJDUMP_SECTION_HEADER                  U64MASK(11)
#define OPTOBJDUMP_DEMANGLE                        U64MASK(12)
#define OPTOBJDUMP_DEBUG_ABBREV                    U64MASK(13)
#define OPTOBJDUMP_DEBUG_ADDR                      U64MASK(14)
#define OPTOBJDUMP_DEBUG_CU_INDEX                  U64MASK(15)
#define OPTOBJDUMP_DEBUG_FRAME                     U64MASK(16)
#define OPTOBJDUMP_DEBUG_FRAME_INTERP              U64MASK(17)
#define OPTOBJDUMP_DEBUG_INFO                      U64MASK(18)
#define OPTOBJDUMP_DEBUG_LINK                      U64MASK(19)
#define OPTOBJDUMP_DEBUG_LINK_FOLLOW               U64MASK(20)
#define OPTOBJDUMP_DEBUG_LINE_RAW                  U64MASK(21)
#define OPTOBJDUMP_DEBUG_LINE_DECODED              U64MASK(22)
#define OPTOBJDUMP_DEBUG_MACRO                     U64MASK(23)
#define OPTOBJDUMP_DEBUG_LOC                       U64MASK(24)
#define OPTOBJDUMP_DEBUG_ARANGES                   U64MASK(25)
#define OPTOBJDUMP_DEBUG_RANGES                    U64MASK(26)
#define OPTOBJDUMP_DEBUG_STR                       U64MASK(27)
#define OPTOBJDUMP_DEBUG_PUBNAMES                  U64MASK(28)
#define OPTOBJDUMP_DEBUG_PUBTYPES                  U64MASK(29)
#define OPTOBJDUMP_TRACE_ARANGES                   U64MASK(30)
#define OPTOBJDUMP_TRACE_ABBREV                    U64MASK(31)
#define OPTOBJDUMP_TRACE_INFO                      U64MASK(32)
#define OPTOBJDUMP_GDB_INDEX                       U64MASK(33)

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

