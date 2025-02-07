#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#define OPT_READELF                    (0)
#define OPT_OBJDUMP                    (1)
#define OPT_OBJCOPY                    (2)

#define OPTREADELF_FILEHEADER          (1 << 0)
#define OPTREADELF_SECTIONGROUPS       (1 << 1)
#define OPTREADELF_SECTIONHEADERS      (1 << 2)
#define OPTREADELF_PROGRAMHEADERS      (1 << 3)
#define OPTREADELF_SYMBOLS             (1 << 4)
#define OPTREADELF_RELOCS              (1 << 5)
#define OPTREADELF_UNWIND              (1 << 6)
#define OPTREADELF_DYNAMIC             (1 << 7)
#define OPTREADELF_HISTOGRAM           (1 << 8)
#define OPTREADELF_ARCH                (1 << 9)
#define OPTREADELF_NOTES               (1 << 10)
#define OPTREADELF_VERSION             (1 << 11)

#define OPTREADELF_ALL                 (OPTREADELF_SYMBOLS | OPTREADELF_RELOCS | OPTREADELF_UNWIND \
                                           | OPTREADELF_DYNAMIC | OPTREADELF_FILEHEADER | OPTREADELF_SECTIONHEADERS \
                                           | OPTREADELF_SECTIONGROUPS | OPTREADELF_PROGRAMHEADERS | OPTREADELF_VERSION \
                                           | OPTREADELF_HISTOGRAM | OPTREADELF_ARCH | OPTREADELF_NOTES)

#define OPTOBJDUMP_DISADDEMBLEALL      (1 << 0)
#define OPTOBJDUMP_SOURCECODE          (1 << 1)
#define OPTOBJDUMP_DYNAMICSYMBOLS      (1 << 2)
#define OPTOBJDUMP_DISASSEMBLE         (1 << 3)
#define OPTOBJDUMP_DEBUGGING           (1 << 4)
#define OPTOBJDUMP_SECTIONS            (1 << 5)
#define OPTOBJDUMP_SYMBOLS             (1 << 6)
#define OPTOBJDUMP_LINENUMBERS         (1 << 7)
#define OPTOBJDUMP_FILEHEADER          (1 << 8)
#define OPTOBJDUMP_PRIVATEHEADER       (1 << 9)
#define OPTOBJDUMP_SECTIONHEADER       (1 << 10)

#define ACT_HEXDUMP                    (0)
#define ACT_STRDUMP                    (1)
#define ACT_RELDUMP                    (2)

typedef struct action_s {
  char mode[3];
  int  action;
  char secname[1024];
  struct action_s *actions;
} action_t, *paction_t;

typedef struct options_s {
  char mode[3];
  int  option;
  char inpname[1024];
  char outname[1024];
  char prgname[1024];

  int  action;
  paction_t actions;
} options_t, *poptions_t;

int get_options_readelf(poptions_t o, int argc, char** argv, char* name);
int get_options_objcopy(poptions_t o, int argc, char** argv, char* name);
int get_options_objdump(poptions_t o, int argc, char** argv, char* name);

int get_options(poptions_t o, int argc, char** argv);

#endif

