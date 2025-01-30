#ifndef __OPTIONS_H_
#define __OPTIONS_H_

#define OPT_READELF                    (0)

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

#define ACT_HEXDUMP                    (0)
#define ACT_STRDUMP                    (1)

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

  int  action;
  paction_t actions;
} options_t, *poptions_t;

int get_options(poptions_t o, int argc, char** argv);

#endif

