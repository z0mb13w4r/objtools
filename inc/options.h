#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#include "defines.h"

#define MODE_OPTIONS            (MODE_PUT0('O') | MODE_PUT1('P') | MODE_PUT2('T'))
#define MODE_ACTIONS            (MODE_PUT0('A') | MODE_PUT1('C') | MODE_PUT2('T'))

#define OPT_READELF                                (0)
#define OPT_OBJCOPY                                (1)
#define OPT_OBJDUMP                                (2)
#define OPT_OBJHASH                                (3)

#define OPTDEBUGELF_DEBUG_ABBREV                   U64MASK(30)
#define OPTDEBUGELF_DEBUG_ADDR                     U64MASK(31)
#define OPTDEBUGELF_DEBUG_CU_INDEX                 U64MASK(32)
#define OPTDEBUGELF_DEBUG_FRAME                    U64MASK(33)
#define OPTDEBUGELF_DEBUG_FRAME_INTERP             U64MASK(34)
#define OPTDEBUGELF_DEBUG_INFO                     U64MASK(35)
#define OPTDEBUGELF_DEBUG_LINK                     U64MASK(36)
#define OPTDEBUGELF_DEBUG_LINK_FOLLOW              U64MASK(37)
#define OPTDEBUGELF_DEBUG_LINE                     U64MASK(38)
#define OPTDEBUGELF_DEBUG_LINE_DECODED             U64MASK(39)
#define OPTDEBUGELF_DEBUG_MACRO                    U64MASK(40)
#define OPTDEBUGELF_DEBUG_LOC                      U64MASK(41)
#define OPTDEBUGELF_DEBUG_ARANGES                  U64MASK(42)
#define OPTDEBUGELF_DEBUG_RANGES                   U64MASK(43)
#define OPTDEBUGELF_DEBUG_STR                      U64MASK(44)
#define OPTDEBUGELF_DEBUG_PUBNAMES                 U64MASK(45)
#define OPTDEBUGELF_DEBUG_PUBTYPES                 U64MASK(46)
#define OPTDEBUGELF_TRACE_ARANGES                  U64MASK(47)
#define OPTDEBUGELF_TRACE_ABBREV                   U64MASK(48)
#define OPTDEBUGELF_TRACE_INFO                     U64MASK(49)
#define OPTDEBUGELF_GDB_INDEX                      U64MASK(50)
#define OPTDEBUGELF_DEBUGGING                      (0x1FFFFFULL << 30)

#define OPTFUNCTION_XOR1                           (1)
#define OPTFUNCTION_XOR255                         (255)
#define OPTFUNCTION_ROT5                           (256)
#define OPTFUNCTION_ROT13                          (257)
#define OPTFUNCTION_ROT18                          (258)

#define OPTPROGRAM_HASH                            U64MASK(55)
#define OPTPROGRAM_DEMANGLE                        U64MASK(56)
#define OPTPROGRAM_LINE_NUMBERS                    U64MASK(57)
#define OPTPROGRAM_DEBUGGING_TAGS                  U64MASK(58)
#define OPTPROGRAM_SOURCE_CODE                     U64MASK(59)
#define OPTPROGRAM_DISASSEMBLE                     U64MASK(60)
#define OPTPROGRAM_CAPSTONE                        U64MASK(61)
#define OPTPROGRAM_HELP                            U64MASK(62)

#define OPTPROGRAM_CAPSTONE_ALL        (OPTPROGRAM_CAPSTONE | OPTPROGRAM_DISASSEMBLE | OPTPROGRAM_SOURCE_CODE | OPTPROGRAM_DEMANGLE)

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

#define OPTREADELF_HEADERS             (OPTREADELF_FILEHEADER | OPTREADELF_SECTIONHEADERS | OPTREADELF_PROGRAMHEADERS)

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
#define OPTOBJDUMP_DYNAMIC_SYMBOLS                 U64MASK(1)
#define OPTOBJDUMP_SECTIONS                        U64MASK(2)
#define OPTOBJDUMP_SYMBOLS                         U64MASK(3)
#define OPTOBJDUMP_FILE_HEADER                     U64MASK(4)
#define OPTOBJDUMP_PRIVATE_HEADER                  U64MASK(5)
#define OPTOBJDUMP_SECTION_HEADER                  U64MASK(6)

#define OPTOBJDUMP_HEADERS             (OPTOBJDUMP_FILE_HEADER | OPTOBJDUMP_PRIVATE_HEADER | OPTOBJDUMP_SECTION_HEADER \
                                           | OPTOBJDUMP_SYMBOLS)

#define OPTOBJHASH_HEADERS                         U64MASK(0)
#define OPTOBJHASH_SECTIONS                        U64MASK(1)

#define OPTOBJHASH_ALL                 (OPTOBJHASH_SECTIONS | OPTOBJHASH_HEADERS)

#define ACT_HEXDUMP                                (0)
#define ACT_STRDUMP8                               (1)
#define ACT_STRDUMP16                              (2)
#define ACT_RELDUMP                                (3)
#define ACT_CODEDUMP                               (4)
#define ACT_ADDSECTION                             (100)
#define ACT_DUMPSECTION                            (101)
#define ACT_UPDATESECTION                          (102)
#define ACT_DISASSEMBLE                            (200)
#define ACT_ROT5                                   (500)
#define ACT_ROT13                                  (501)
#define ACT_ROT18                                  (502)
#define ACT_ADD8                                   (503)
#define ACT_ADD16                                  (504)
#define ACT_ADD32                                  (505)
#define ACT_NOT8                                   (506)
#define ACT_NOT16                                  (507)
#define ACT_NOT32                                  (508)
#define ACT_ROL8                                   (509)
#define ACT_ROL16                                  (510)
#define ACT_ROL32                                  (511)
#define ACT_ROR8                                   (512)
#define ACT_ROR16                                  (513)
#define ACT_ROR32                                  (514)
#define ACT_SHL8                                   (515)
#define ACT_SHL16                                  (516)
#define ACT_SHL32                                  (517)
#define ACT_SHR8                                   (518)
#define ACT_SHR16                                  (519)
#define ACT_SHR32                                  (520)
#define ACT_SUB16                                  (521)
#define ACT_SUB8                                   (522)
#define ACT_SUB32                                  (523)
#define ACT_XOR8                                   (524)
#define ACT_XOR16                                  (525)
#define ACT_XOR32                                  (526)

typedef struct action_s {
  smode_t  mode;
  int      action;
  char     secname[256];
  char     outname[PATH_MAX];
  uint64_t value;
  struct action_s *actions;
} action_t, *paction_t;

typedef struct options_s {
  smode_t mode;
  imode_t option;
  imode_t action;
  imode_t convert;

  union {
    char inpname[PATH_MAX];
    char inpname0[PATH_MAX];
  };
  union {
    char outname[PATH_MAX];
    char inpname1[PATH_MAX];
  };
  char prgname[256];

  uint64_t  saddress; /* --start-address */
  uint64_t  eaddress; /* --stop-address */

  paction_t actions;
} options_t, *poptions_t;

int get_options_readelf(poptions_t o, int argc, char** argv, char* name);
int get_options_objcopy(poptions_t o, int argc, char** argv, char* name);
int get_options_objdump(poptions_t o, int argc, char** argv, char* name);
int get_options_objhash(poptions_t o, int argc, char** argv, char* name);

int get_options(poptions_t o, int argc, char** argv);

int isactions(handle_t p);
int isoptions(handle_t p);

handle_t amalloc();
handle_t omalloc();
handle_t afree(handle_t p);
handle_t ofree(handle_t p);

#endif

