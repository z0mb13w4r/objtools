#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#include "externs.h"

#define MODE_OPTIONS            (MODE_PUT0('O') | MODE_PUT1('P') | MODE_PUT2('T'))
#define MODE_ACTIONS            (MODE_PUT0('A') | MODE_PUT1('C') | MODE_PUT2('T'))

#define OPTDISASSEMBLE_ATT                         U64MASK(0)
#define OPTDISASSEMBLE_INTEL                       U64MASK(1)
#define OPTDISASSEMBLE_ATT_MNEMONIC                U64MASK(2)
#define OPTDISASSEMBLE_INTEL_MNEMONIC              U64MASK(3)
#define OPTDISASSEMBLE_X86_64                      U64MASK(4)
#define OPTDISASSEMBLE_I386                        U64MASK(5)
#define OPTDISASSEMBLE_I8086                       U64MASK(6)
#define OPTDISASSEMBLE_AMD64                       U64MASK(7)
#define OPTDISASSEMBLE_INTEL64                     U64MASK(8)
#define OPTDISASSEMBLE_ARM32                       U64MASK(9)
#define OPTDISASSEMBLE_ARM64                       U64MASK(10)
#define OPTDISASSEMBLE_RISCV32                     U64MASK(11)
#define OPTDISASSEMBLE_RISCV64                     U64MASK(12)
#define OPTDISASSEMBLE_ADDR16                      U64MASK(13)
#define OPTDISASSEMBLE_ADDR32                      U64MASK(14)
#define OPTDISASSEMBLE_ADDR64                      U64MASK(15)
#define OPTDISASSEMBLE_DATA16                      U64MASK(16)
#define OPTDISASSEMBLE_DATA32                      U64MASK(17)
#define OPTDISASSEMBLE_ENHANCED                    U64MASK(18)
#define OPTDISASSEMBLE_VERBOSE                     U64MASK(19)

#define OPTDWARF_DEBUG_ABBREV                      U64MASK(30)
#define OPTDWARF_DEBUG_ADDR                        U64MASK(31)
#define OPTDWARF_DEBUG_CU_INDEX                    U64MASK(32)
#define OPTDWARF_DEBUG_FRAME                       U64MASK(33)
#define OPTDWARF_DEBUG_FRAME_DECODED               U64MASK(34)
#define OPTDWARF_DEBUG_INFO                        U64MASK(35)
#define OPTDWARF_DEBUG_LINK                        U64MASK(36)
#define OPTDWARF_DEBUG_LINK_FOLLOW                 U64MASK(37)
#define OPTDWARF_DEBUG_LINE                        U64MASK(38)
#define OPTDWARF_DEBUG_LINE_DECODED                U64MASK(39)
#define OPTDWARF_DEBUG_MACRO                       U64MASK(40)
#define OPTDWARF_DEBUG_LOC                         U64MASK(41)
#define OPTDWARF_DEBUG_ARANGES                     U64MASK(42)
#define OPTDWARF_DEBUG_RANGES                      U64MASK(43)
#define OPTDWARF_DEBUG_STR                         U64MASK(44)
#define OPTDWARF_DEBUG_PUBNAMES                    U64MASK(45)
#define OPTDWARF_DEBUG_PUBTYPES                    U64MASK(46)
#define OPTDWARF_TRACE_ARANGES                     U64MASK(47)
#define OPTDWARF_TRACE_ABBREV                      U64MASK(48)
#define OPTDWARF_TRACE_INFO                        U64MASK(49)
#define OPTDWARF_GDB_INDEX                         U64MASK(50)
#define OPTDWARF_ENHANCED                          U64MASK(51)
#define OPTDWARF_VERBOSE                           U64MASK(52)
#define OPTDWARF_TAGS                              U64MASK(53)
#define OPTDWARF_DEBUGGING                        (0x1FFFFFULL << 30)

#define OPTDWARF_DEBUGGING_DBGS        (OPTDWARF_DEBUG_FRAME_DECODED | OPTDWARF_DEBUG_ARANGES | OPTDWARF_DEBUG_INFO \
                                           | OPTDWARF_DEBUG_ABBREV | OPTDWARF_DEBUG_STR | OPTDWARF_DEBUG_MACRO \
                                           | /*OPTDWARF_DEBUG_LOC |*/ OPTDWARF_DEBUG_LINE_DECODED)

#define OPTDWARF_DEBUGGING_TAGS        (OPTDWARF_DEBUGGING_DBGS | OPTDWARF_TAGS)

#define OPTDWARF_DEBUG_LINE_MASK       (OPTDWARF_DEBUG_LINE | OPTDWARF_DEBUG_LINE_DECODED)
#define OPTDWARF_DEBUG_FRAME_MASK      (OPTDWARF_DEBUG_FRAME | OPTDWARF_DEBUG_FRAME_DECODED)

#define OPTPROGRAM_DEBUGLEVEL1                     U64MASK(47)
#define OPTPROGRAM_DEBUGLEVEL2                     U64MASK(48)
#define OPTPROGRAM_DEBUGLEVEL3                     U64MASK(49)
#define OPTPROGRAM_ENHANCED                        U64MASK(50)
#define OPTPROGRAM_VERBOSE                         U64MASK(51)
#define OPTPROGRAM_HASH                            U64MASK(52)
#define OPTPROGRAM_ENTROPY                         U64MASK(53)
#define OPTPROGRAM_DEMANGLE                        U64MASK(54)
#define OPTPROGRAM_NO_ADDRESSES                    U64MASK(55)
#define OPTPROGRAM_NO_SHOW_RAW_INSN                U64MASK(56)
#define OPTPROGRAM_PREFIX_ADDR                     U64MASK(57)
#define OPTPROGRAM_LINE_NUMBERS                    U64MASK(58)
#define OPTPROGRAM_SOURCE_CODE                     U64MASK(59)
#define OPTPROGRAM_DISASSEMBLE                     U64MASK(60)
#define OPTPROGRAM_CAPSTONE                       (U64MASK(61) | OPTPROGRAM_DISASSEMBLE)
#define OPTPROGRAM_VERSION                         U64MASK(62)
#define OPTPROGRAM_HELP                            U64MASK(63)

#define OPTPROGRAM_INFO                (OPTPROGRAM_HASH | OPTPROGRAM_ENTROPY)

#define OPTREADELF_FILEHEADER                      U64MASK(0)
#define OPTREADELF_SECTIONGROUPS                   U64MASK(1)
#define OPTREADELF_SECTIONHEADERS                  U64MASK(2)
#define OPTREADELF_SECTIONDETAILS                  U64MASK(3)
#define OPTREADELF_PROGRAMHEADERS                  U64MASK(4)
#define OPTREADELF_SYMBOLS                         U64MASK(5)
#define OPTREADELF_RELOCS                          U64MASK(6)
#define OPTREADELF_UNWIND                          U64MASK(7)
#define OPTREADELF_DYNAMIC                         U64MASK(8)
#define OPTREADELF_HISTOGRAM                       U64MASK(9)
#define OPTREADELF_ARCHSPECIFIC                    U64MASK(10)
#define OPTREADELF_NOTES                           U64MASK(11)
#define OPTREADELF_VERSION                         U64MASK(12)
#define OPTREADELF_USEDYNAMIC                      U64MASK(13)

#define OPTREADELF_HEADERS             (OPTREADELF_FILEHEADER | OPTREADELF_SECTIONHEADERS | OPTREADELF_PROGRAMHEADERS)

#define OPTREADELF_ALL                 (OPTREADELF_SYMBOLS | OPTREADELF_RELOCS | OPTREADELF_UNWIND \
                                           | OPTREADELF_DYNAMIC | OPTREADELF_FILEHEADER | OPTREADELF_SECTIONHEADERS \
                                           | OPTREADELF_SECTIONGROUPS | OPTREADELF_PROGRAMHEADERS | OPTREADELF_VERSION \
                                           | OPTREADELF_HISTOGRAM | OPTREADELF_ARCHSPECIFIC | OPTREADELF_NOTES)

#define OPTREADELF_SECTIONMASK         (OPTREADELF_SECTIONHEADERS | OPTREADELF_SECTIONDETAILS)

#define OPTREADPE_DOSHEADER                        U64MASK(0)
#define OPTREADPE_NTHEADER                         U64MASK(1)
#define OPTREADPE_SECTIONGROUPS                    U64MASK(2)
#define OPTREADPE_SECTIONHEADERS                   U64MASK(3)
#define OPTREADPE_RESOURCE                         U64MASK(4)
#define OPTREADPE_CONFIG                           U64MASK(5)
#define OPTREADPE_DEBUG                            U64MASK(6)
#define OPTREADPE_RUNTIME                          U64MASK(7)
#define OPTREADPE_RELOCS                           U64MASK(8)
#define OPTREADPE_IAT                              U64MASK(9)
#define OPTREADPE_EAT                              U64MASK(10)
#define OPTREADPE_VERSION                          U64MASK(11)

#define OPTREADPE_HEADERS              (OPTREADPE_DOSHEADER | OPTREADPE_NTHEADER | OPTREADPE_SECTIONHEADERS)

#define OPTREADPE_ALL                  (OPTREADPE_DOSHEADER | OPTREADPE_NTHEADER | OPTREADPE_SECTIONGROUPS \
                                           | OPTREADPE_SECTIONHEADERS | OPTREADPE_RUNTIME | OPTREADPE_RELOCS \
                                           | OPTREADPE_IAT | OPTREADPE_EAT)

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
#define OPTOBJCOPY_STRIP_UNNEEDED                  U64MASK(22)
#define OPTOBJCOPY_WEAKEN                          U64MASK(23)
#define OPTOBJCOPY_WRITABLE_TEXT                   U64MASK(24)
#define OPTOBJCOPY_DUMP_SECTIONS_ALL               U64MASK(25)

#define OPTOBJDUMP_DISASSEMBLE_ALL                (U64MASK(0) | OPTPROGRAM_DISASSEMBLE)
#define OPTOBJDUMP_DYNAMIC_SYMBOLS                 U64MASK(1)
#define OPTOBJDUMP_DYNAMIC_RELOC                   U64MASK(2)
#define OPTOBJDUMP_SECTIONS                        U64MASK(3)
#define OPTOBJDUMP_SYMBOLS                         U64MASK(4)
#define OPTOBJDUMP_FILE_HEADER                     U64MASK(5)
#define OPTOBJDUMP_PRIVATE_HEADER                  U64MASK(6)
#define OPTOBJDUMP_SECTION_HEADER                  U64MASK(7)
#define OPTOBJDUMP_RELOC                           U64MASK(8)
#define OPTOBJDUMP_DEBUGGING_DBGS                  U64MASK(9)
#define OPTOBJDUMP_DEBUGGING_TAGS                  U64MASK(10)

#define OPTOBJDUMP_HEADERS             (OPTOBJDUMP_FILE_HEADER | OPTOBJDUMP_PRIVATE_HEADER | OPTOBJDUMP_SECTION_HEADER \
                                           | OPTOBJDUMP_SYMBOLS | OPTOBJDUMP_RELOC)

#define OPTOBJDUMP_CAPSTONE_ALL        (OPTOBJDUMP_DISASSEMBLE_ALL | OPTPROGRAM_CAPSTONE)

#define OPTOBJHASH_HEADERS                         U64MASK(0)
#define OPTOBJHASH_SECTIONS                        U64MASK(1)
#define OPTOBJHASH_MD5                             U64MASK(2)
#define OPTOBJHASH_SHA1                            U64MASK(3)
#define OPTOBJHASH_SHA256                          U64MASK(4)
#define OPTOBJHASH_SHA512                          U64MASK(5)
#define OPTOBJHASH_SSDEEP                          U64MASK(6)
#define OPTOBJHASH_FILESIZE                        U64MASK(7)

#define OPTOBJHASH_ALL                 (OPTOBJHASH_SECTIONS | OPTOBJHASH_HEADERS)
#define OPTOBJHASH_HASHES              (OPTOBJHASH_MD5 | OPTOBJHASH_SHA1 | OPTOBJHASH_SHA256 | OPTOBJHASH_SHA512 \
                                           | OPTOBJHASH_SSDEEP)

#define OPTUSE_SIZE                                U64MASK(60)
#define OPTUSE_ADDRESS                             U64MASK(61)
#define OPTUSE_SECTION                             U64MASK(62)
#define OPTUSE_FILESPEC                            U64MASK(63)
#define OPTUSE_MASK                    (OPTUSE_SIZE | OPTUSE_ADDRESS | OPTUSE_SECTION | OPTUSE_FILESPEC)

#define ACT_HEXDUMP                                (0)
#define ACT_STRDUMP8                               (1)
#define ACT_STRDUMP16                              (2)
#define ACT_RELDUMP                                (3)
#define ACT_CODEDUMP                               (4)
#define ACT_DISASSEMBLE                            (5)
#define ACT_PIECEWISE                              (6)
#define ACT_THRESHOLD                              (7)

#define ACT_ADDSECTION                             (100)
#define ACT_DUMPSECTION                            (101)
#define ACT_UPDATESECTION                          (102)

#define ACT_ROT5                                   (200)
#define ACT_ROT13                                  (201)
#define ACT_ROT18                                  (202)
#define ACT_ROT47                                  (203)

#define ACT_ADD8                                   (301)
#define ACT_ADD16                                  (302)
#define ACT_ADD32                                  (303)
#define ACT_NOT8                                   (304)
#define ACT_NOT16                                  (305)
#define ACT_NOT32                                  (306)
#define ACT_ROL8                                   (307)
#define ACT_ROL16                                  (308)
#define ACT_ROL32                                  (309)
#define ACT_ROR8                                   (310)
#define ACT_ROR16                                  (311)
#define ACT_ROR32                                  (312)
#define ACT_SHL8                                   (313)
#define ACT_SHL16                                  (314)
#define ACT_SHL32                                  (315)
#define ACT_SHR8                                   (316)
#define ACT_SHR16                                  (317)
#define ACT_SHR32                                  (318)
#define ACT_SUB16                                  (319)
#define ACT_SUB8                                   (320)
#define ACT_SUB32                                  (321)
#define ACT_XOR8                                   (322)
#define ACT_XOR16                                  (323)
#define ACT_XOR32                                  (324)
#define ACT_DEC                                    (325)
#define ACT_INC                                    (326)

#define ACT_BASE32D                                (400)
#define ACT_BASE32E                                (401)
#define ACT_BASE58D                                (402)
#define ACT_BASE58E                                (403)
#define ACT_BASE64D                                (404)
#define ACT_BASE64E                                (405)
#define ACT_BASE85D                                (406)
#define ACT_BASE85E                                (407)
#define ACT_VIGENERED                              (408 | ACT_USESTRING)
#define ACT_VIGENEREE                              (409 | ACT_USESTRING)

#define ACT_BIN8D                                  (500)
#define ACT_BIN8E                                  (501)
#define ACT_BIN16D                                 (502)
#define ACT_BIN16E                                 (503)
#define ACT_BIN32D                                 (504)
#define ACT_BIN32E                                 (505)
#define ACT_DEC8D                                  (506)
#define ACT_DEC8E                                  (507)
#define ACT_DEC16D                                 (508)
#define ACT_DEC16E                                 (509)
#define ACT_DEC32D                                 (510)
#define ACT_DEC32E                                 (511)
#define ACT_HEX8D                                  (512)
#define ACT_HEX8E                                  (513)
#define ACT_HEX16D                                 (514)
#define ACT_HEX16E                                 (515)
#define ACT_HEX32D                                 (516)
#define ACT_HEX32E                                 (517)

#define ACT_ROL8BRUTE                              (700)
#define ACT_ROL16BRUTE                             (701)
#define ACT_ROR8BRUTE                              (702)
#define ACT_ROR16BRUTE                             (703)
#define ACT_SHL8BRUTE                              (704)
#define ACT_SHL16BRUTE                             (705)
#define ACT_SHR8BRUTE                              (706)
#define ACT_SHR16BRUTE                             (707)
#define ACT_XOR8BRUTE                              (708)
#define ACT_XOR16BRUTE                             (709)

#define ACT_ZLIB                                   (800)

#define ACT_USEIVALUE                              U32MASK(29)
#define ACT_USESTRING                              U32MASK(30)

#define PICK_VERBOSE(x,y,z)            (MODE_ISANY((x)->ocdump, OPTDWARF_VERBOSE) ? (y) : (z))
#define PICK_ENHANCED(x,y,z)           (MODE_ISANY((x)->ocdump, OPTDWARF_ENHANCED) ? (y) : (z))

typedef struct action_s {
  smode_t  mode;
  int      action;
  union {
    char     sname[256];
    char     secname[256];
  };
  union {
    char     inpname[PATH_MAX];
    char     outname[PATH_MAX];
  };
  union {
    size_t   size;
    int64_t  ivalue;
    uint64_t uvalue;
  };
  struct action_s *actions;
} action_t, *paction_t;

typedef struct options_s {
  smode_t mode;
  imode_t action;
  imode_t ocdump;

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

typedef struct imodeswap_s {
  imode_t mode1;
  imode_t mode2;
} imodeswap_t, *pimodeswap_t;

int isactions(handle_t p);
int isoptions(handle_t p);

handle_t amalloc();
handle_t omalloc();
handle_t afree(handle_t p);
handle_t ofree(handle_t p);

int odeath(poptions_t o, const char* name, const char* argument);

int usage_synopsis0(poptions_t o, const char* name, const args_t args[]);
int usage_synopsis1(poptions_t o, const char* name, const args_t args[], const char* more0, const char* more1);
int usage_synopsis2(poptions_t o, const char* name, const args_t args[], const char* more0, const char* more1);
int usage_synopsis3(poptions_t o, const char* name, const args_t args[]);

int usage_description(poptions_t o, const char* name, const args_t args[], const char* desc[]);

int usage_options0(poptions_t o, const char* name, const args_t args[]);
int usage_options1(poptions_t o, const char* name, const args_t args[], const char* more0, const char* more1);
int usage_options2(poptions_t o, const char* name, const args_t args[], const char* more0, const char* more1);

int usage_seealso(poptions_t o, const char* name, const args_t args[]);

int usage_copyright(poptions_t o, const char* name, const args_t args[]);

int usage_name(poptions_t o, const char* name, const args_t args[], const char* desc);

int version0(poptions_t o, const char* name, const args_t args[]);

int breakup_args(char* args, char* dst0, const size_t dst0size, char* dst1, const size_t dst1size);

int oinsert(handle_t o, handle_t p, const int action);
int oinsertsname(handle_t o, const int action, const char *sname);
int oinsertvalue(handle_t o, const int action, const uint64_t value);

imode_t get_options1(poptions_t o, const args_t args[], const char *argv);
imode_t set_options1(poptions_t o, const args_t args[]);
imode_t get_options2(poptions_t o, const args_t args[], const char *argv);
imode_t get_ocdump(poptions_t o, const imodeswap_t args[], imode_t action);

extern const args_t zDISASSEMBLEARGS[];
extern const args_t zDEBUGELFARGS[];

#endif

