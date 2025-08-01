#include "buffer.h"
#include "printf.h"
#include "objcopy.h"
#include "options.h"

static const args_t OBJCOPYARGS[] = {
  {'H', "--help",                           OPTPROGRAM_HELP},
  {'V', "--version",                        OPTPROGRAM_VERSION},
  {0,   "--adjust-warnings",                OPTOBJCOPY_CHANGE_WARNINGS},
  {0,   "--change-warnings",                OPTOBJCOPY_CHANGE_WARNINGS},
  {0,   "--debugging",                      OPTOBJCOPY_DEBUGGING},
  {0,   "--decompress-debug-sections",      OPTOBJCOPY_DECOMPRESS_DEBUG_SECTIONS},
  {'U', "--disable-deterministic-archives", OPTOBJCOPY_DISABLE_DETERMINISTIC_ARCHIVES},
  {'x', "--discard-all",                    OPTOBJCOPY_DISCARD_ALL},
  {'X', "--discard-locals",                 OPTOBJCOPY_DISCARD_LOCALS},
  {'D', "--enable-deterministic-archives",  OPTOBJCOPY_ENABLE_DETERMINISTIC_ARCHIVES},
  {0,   "--extract-dwo",                    OPTOBJCOPY_EXTRACT_DWO},
  {0,   "--extract-symbol",                 OPTOBJCOPY_EXTRACT_SYMBOL},
  {0,   "--impure",                         OPTOBJCOPY_IMPURE},
  {0,   "--keep-file-symbols",              OPTOBJCOPY_KEEP_FILE_SYMBOLS},
  {0,   "--localize-hidden",                OPTOBJCOPY_LOCALIZE_HIDDEN},
  {'M', "--merge-notes",                    OPTOBJCOPY_MERGE_NOTES},
  {0,   "--no-merge-notes",                 OPTOBJCOPY_NO_MERGE_NOTES},
  {0,   "--no-adjust-warnings",             OPTOBJCOPY_NO_CHANGE_WARNINGS},
  {0,   "--no-change-warnings",             OPTOBJCOPY_NO_CHANGE_WARNINGS},
  {0,   "--only-keep-debug",                OPTOBJCOPY_ONLY_KEEP_DEBUG},
  {'p', "--preserve-dates",                 OPTOBJCOPY_PRESERVE_DATES},
  {0,   "--pure",                           OPTOBJCOPY_PURE},
  {0,   "--readonly-text",                  OPTOBJCOPY_READONLY_TEXT},
  {0,   "--strip-section-headers",          OPTOBJCOPY_STRIP_SECTION_HEADERS},
  {'S', "--strip-all",                      OPTOBJCOPY_STRIP_ALL},
  {'g', "--strip-debug",                    OPTOBJCOPY_STRIP_DEBUG},
  {0,   "--strip-dwo",                      OPTOBJCOPY_STRIP_DWO},
  {0,   "--strip-unneeded",                 OPTOBJCOPY_STRIP_UNNEEDED},
  {'v', "--verbose",                        OPTOBJCOPY_VERBOSE},
  {0,   "--weaken",                         OPTOBJCOPY_WEAKEN},
  {0,   "--writable-text",                  OPTOBJCOPY_WRITABLE_TEXT},
  {0,   "--dump-sections-all",              OPTOBJCOPY_DUMP_SECTIONS_ALL},
  {0, NULL},
};

static int get_options_objcopy(poptions_t o, int argc, char** argv, char* name) {
  if (argc < 1) {
    return -1;
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      if (0 == strcmp(argv[i], "--add-section")) {
        paction_t p = amalloc();
        if (p && ECODE_ISOK(breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname)))) {
          oinsert(o, p, ACT_ADDSECTION);
        }
      } else if (0 == strcmp(argv[i], "--dump-section")) {
        paction_t p = amalloc();
        if (p && ECODE_ISOK(breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname)))) {
          oinsert(o, p, ACT_DUMPSECTION);
        }
      } else if (0 == strcmp(argv[i], "--update-section")) {
        paction_t p = amalloc();
        if (p && ECODE_ISOK(breakup_args(argv[++i], p->secname, NELEMENTS(p->secname), p->outname, NELEMENTS(p->outname)))) {
          oinsert(o, p, ACT_UPDATESECTION);
        }
      } else {
        o->action |= get_options2(o, OBJCOPYARGS, argv[i]);
      }
    } else if ('-' == argv[i][0]) {
      o->action |= get_options1(o, OBJCOPYARGS, argv[i]);
    } else {
      strncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    }
  }

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, "objcopy-ng", OBJCOPYARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage0(o, "objcopy-ng", OBJCOPYARGS);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int r = -1;
  poptions_t o = omalloc();
  if (o) {
    r = get_options_objcopy(o, argc, argv, argv[0]);
    if (0 == r) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        r = objcopy(p, o);
      }

      xfree(p);
    }
  }

  xfree(o);
  return r;
}

