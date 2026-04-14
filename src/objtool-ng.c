#include "buffer.h"
#include "printf.h"
#include "readar.h"
#include "readpe.h"
#include "scripts.h"
#include "readelf.h"
#include "objcopy.h"
#include "objhash.h"
#include "options.h"

#define THIS_NAME "objtools-ng"

static const char *zELFDESCRIPTION[] = {
  "displays information about one or more ELF format object files. The options",
  "control what particular information to display.",
  "",
  "elffile... are the object files to be examined. 32-bit and 64-bit ELF files",
  "are supported, as are archives containing ELF files.",
  0
};

static const args_t zARGS[] = {
  {'H', "--help",            OPTPROGRAM_HELP,              NULL},
  {'v', "--version",         OPTPROGRAM_VERSION,           NULL},
  {'C', "--demangle",        OPTPROGRAM_DEMANGLE,          NULL},
  {'x', "--hex-dump",        0,                            zHEXDUMP,                 OPTUSE_SECTION},
  {'Z', "--code-dump",       0,                            zCODEDUMP,                OPTUSE_SECTION},
  {'p', "--string-dump",     0,                            zSTRDUMP,                 OPTUSE_SECTION},
  {'U', "--unicode-dump",    0,                            zSTRDUMP,                 OPTUSE_SECTION},
  {'R', "--relocated-dump",  0,                            zHEXDUMP,                 OPTUSE_SECTION},
  {'z', "--decompress",      0,                            NULL},
  {'m', "--disassemble",     0,                            NULL},
  {'I', "--info",            OPTPROGRAM_INFO,              NULL},
  {'X', "--hash",            OPTPROGRAM_HASH,              NULL},
  {'E', "--entropy",         OPTPROGRAM_ENTROPY,           NULL},
  {0, NULL}
};

static const imodeswap_t zSWAP[] = {
  {OPTPROGRAM_ENHANCED,        OPTDISASSEMBLE_ENHANCED},
  {OPTPROGRAM_ENHANCED,        OPTDWARF_ENHANCED},
  {OPTPROGRAM_VERBOSE,         OPTDISASSEMBLE_VERBOSE},
  {OPTPROGRAM_VERBOSE,         OPTDWARF_VERBOSE},
  {0, 0}
};

static int usage(poptions_t o, const char* name, const args_t args[], const int ecode) {
  int n = 0;
  n += usage_name(o, name, args, zDESCRIPTION);
  n += usage_synopsis0(o, name, args);
  n += usage_synopsis1(o, name, zDEBUGELFARGS, "-w", "--debug-dump");
  n += usage_synopsis2(o, name, zDISASSEMBLEARGS, "-M", "--disassembler-options");
  n += usage_synopsis2(o, name, zSCRIPTCOMMANDS, "-T", "--script");
  n += printf_eol();
  n += usage_description(o, name, args, zELFDESCRIPTION);
  n += usage_options0(o, name, args);
  n += usage_options1(o, name, zDEBUGELFARGS, "-w", "--debug-dump");
  n += usage_options2(o, name, zDISASSEMBLEARGS, "-M", "--disassembler-options");
  n += usage_options2(o, name, zSCRIPTCOMMANDS, "-T", "--script");
  n += usage_seealso(o, name, args);
  n += usage_copyright(o, name, args);

  return ecode;
}

static int get_options_objtools(poptions_t o, int argc, char** argv, char* name) {
  if (0 == argc) {
    return usage(o, THIS_NAME, zARGS, ECODE_ARGUMENTS);
  }

  strname(o->prgname, name);
  printf_errname(o->prgname);

  for (int i = 0; i < argc; ++i) {
    if ('-' == argv[i][0] && '-' == argv[i][1]) {
      MALLOCA(char, arg0, 1024);
      MALLOCA(char, arg1, 1024);

      if (ECODE_ISOK(breakup_args(argv[i], arg0, NELEMENTS(arg0), arg1, NELEMENTS(arg1)))) {
        if (0 == xstrcmp(arg0, "--debug-dump")) {
          imode_t ocdump = get_options2(o, zDEBUGELFARGS, arg1);
          if (0 == ocdump) {
            return odeath(o, THIS_NAME, arg1);
          }
          o->ocdump |= ocdump;
        } else if (0 == xstrcmp(arg0, "--hex-dump")) {
          oinsertsname(o, ACT_HEXDUMP, arg1);
        } else if (0 == xstrcmp(arg0, "--string-dump")) {
          oinsertsname(o, ACT_STRDUMP8, arg1);
        } else if (0 == xstrcmp(arg0, "--unicode-dump")) {
          oinsertsname(o, ACT_STRDUMP16, arg1);
        } else if (0 == xstrcmp(arg0, "--relocated-dump")) {
          oinsertsname(o, ACT_RELDUMP, arg1);
        } else if (0 == xstrcmp(arg0, "--code-dump")) {
          oinsertsname(o, ACT_CODEDUMP, arg1);
        } else if (0 == xstrcmp(arg0, "--disassemble")) {
          oinsertsname(o, ACT_DISASSEMBLE, arg1);
        } else if (0 == xstrcmp(arg0, "--decompress")) {
          oinsertsname(o, ACT_ZLIB, arg1);
        } else if (0 == xstrcmp(arg0, "--script")) {
          sinsert(o, arg1, zSCRIPTCOMMANDS, NULL);
        } else {
          return odeath(o, THIS_NAME, arg0 + 2);
        }
      } else {
        imode_t action = get_options2(o, zARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 2);
        }
        o->action |= action;
      }
    } else if ('-' == argv[i][0]) {
      if (argv[i][0] == '-' && argv[i][1] == 'w') {
        imode_t ocdump = get_options1(o, zDEBUGELFARGS, argv[i] + 1);
        if (0 == ocdump && argv[i][2]) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        o->ocdump |= ocdump ? ocdump : set_options1(o, zDEBUGELFARGS);
      } else if (0 == xstrcmp(argv[i], "-x")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsname(o, ACT_HEXDUMP, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-p")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsname(o, ACT_STRDUMP8, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-U")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsname(o, ACT_STRDUMP16, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-R")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsname(o, ACT_RELDUMP, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-Z")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsname(o, ACT_CODEDUMP, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-m")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsname(o, ACT_DISASSEMBLE, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-z")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        oinsertsname(o, ACT_ZLIB, argv[++i]);
      } else if (0 == xstrcmp(argv[i], "-T")) {
        if (argc <= (i + 1)) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        sinsert(o, argv[++i], zSCRIPTCOMMANDS, NULL);
      } else {
        imode_t action = get_options1(o, zARGS, argv[i]);
        if (0 == action) {
          return odeath(o, THIS_NAME, argv[i] + 1);
        }
        o->action |= action;
      }
    } else if (0 == o->inpname[0]) {
      xstrncpy(o->inpname, argv[i], NELEMENTS(o->inpname));
    } else {
      return odeath(o, THIS_NAME, argv[i]);
    }
  }

  o->ocdump |= get_ocdump(o, zSWAP, o->action);

  if (o->action & OPTPROGRAM_VERSION) {
    return version0(o, THIS_NAME, zARGS);
  }

  if (o->action & OPTPROGRAM_HELP) {
    return usage(o, THIS_NAME, zARGS, ECODE_OK);
  }

  return ECODE_OK;
}

int main(int argc, char* argv[]) {
  int r = -1;

  poptions_t o = omalloc();
  if (o) {
    r = get_options_objtools(o, argc - 1, argv + 1, argv[0]);
    if (0 == r) {
      pbuffer_t p = bopen(o->inpname);
      if (p) {
        if (isAR(p)) {
          r = readar(p, o);
        } else if (isPE(p)) {
          r = readpe(p, o);
        } else if (isELF(p)) {
          r = readelf(p, o);
        }
      } else {
        printf_e("'%s': no such file.", o->inpname);
      }

      bfree(p);
    }
  }

  ofree(o);
  return r;
}

