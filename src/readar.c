#include "arcode.h"
#include "printf.h"
#include "readar.h"
#include "readelf.h"
#include "memfind.h"
#include "objutils.h"

static const int MAXSIZE = 36;
static int symindex = -1;

static int dump_achive0(const pbuffer_t p, const poptions_t o, const int index, const size_t size) {
  int n0 = 0;
  int n1 = 0;

  if (MODE_ISANY(o->action, OPTPROGRAM_VERBOSE)) {
    const imode_t USE_FHEXNN = MODE_ISANY(size, MEMFIND_64BIT) ? USE_FHEX64 : USE_FHEX32;

    handle_t p0 = ecget_archive(p, index, size);
    if (p0) {
      n0 += printf_text("ARCHIVE BODY", USE_LT | USE_COLON | USE_EOL);

      struct ar_hdr* p1 = fgetp(p0, sizeof(struct ar_hdr));
      if (p1) {
        size_t count = fgetuNN(p0);
        n0 += printf_text("Count", USE_TAB | USE_COLON | SET_PAD(MAXSIZE - 1));
        n0 += printf_nice(count, USE_DEC | USE_EOL);

        n0 += printf_text("INDICES", USE_TAB | USE_COLON | USE_EOL);
        for (size_t i = 0; i < count; ++i) {
          if (fisokNN(p0)) {
            n1  = printf_text("Index", USE_TAB2);
            n1 += printf_nice(i, USE_DEC | USE_SB | USE_COLON | USE_NOSPACE);
            n0 += printf_pack(MAXSIZE - 1 - n1) + n1;
            n0 += printf_nice(fgetuNN(p0), USE_FHEXNN | USE_EOL);
          } else {
            printf_e("missing indices from archive");
            break;
          }
        }

        n0 += printf_sore(fget(p0), fgetsize(p0), USE_HEXDUMP);
      }

      ffree(p0);
    }
  }

  return n0;
}

static int dump_achive1(const pbuffer_t p, const poptions_t o, const int index, const size_t size) {
  int n = 0;
  symindex = index;

  if (MODE_ISANY(o->action, OPTPROGRAM_VERBOSE)) {
    handle_t p0 = ecget_archive(p, index, size);
    if (p0) {
      n += printf_text("ARCHIVE BODY", USE_LT | USE_COLON | USE_EOL);

      struct ar_hdr* p1 = fgetp(p0, sizeof(struct ar_hdr));
      if (p1) {
        n += printf_sore(fget(p0), fgetsize(p0), USE_HEXDUMP);
      }

      ffree(p0);
    }
  }

  return n;
}

static int dump_achive2(const pbuffer_t p, const poptions_t o, const int index, const size_t size) {
  int n = 0;

  handle_t p0 = ecget_archive(p, index, size);
  handle_t q0 = ecget_archive(p, symindex, size);
  if (p0) {
    struct ar_hdr* p1 = fgetp(p0, sizeof(struct ar_hdr));
    struct ar_hdr* q1 = fgetp(q0, sizeof(struct ar_hdr));
    if (p1 && q1) {
//printf_sore(fget(p0), fgetsize(p0), USE_HEXDUMP);
//printf_sore(fget(q0), fgetsize(q0), USE_HEXDUMP);
      const size_t offset = decb(p1->ar_name + 1, sizeof(p1->ar_name) - 1);
      char* name = fgrabsequence(q0, offset, '/');
//printf("offset=%ld:%s\n", offset, name);
      handle_t p2 = bcalloc(fget(p0), fgetsize(p0));
      if (p2) {
        n += dumpelf(p2, o, name);
        bfree(p2);
      }
      xfree(name);
    }

    ffree(p0);
  }

  return n;
}

static int dump_achive3(const pbuffer_t p, const poptions_t o, const int index, const size_t size) {
  int n = 0;

  handle_t p0 = ecget_archive(p, index, size);
  if (p0) {
    struct ar_hdr* p1 = fgetp(p0, sizeof(struct ar_hdr));
    if (p1) {
//printf_sore(fget(p0), fgetsize(p0), USE_HEXDUMP);
//printf("offset=%ld:%s\n", offset, p1->ar_name);
      char* name = xstrnnab(p1->ar_name, sizeof(p1->ar_name), '/');
      handle_t p2 = bcalloc(fget(p0), fgetsize(p0));
      if (p2) {
        n += dumpelf(p2, o, name);
        bfree(p2);
      }
      xfree(name);
    }

    ffree(p0);
  }

  return n;
}

static int dump_archive(const pbuffer_t p, const poptions_t o, const int index) {

  int n = 0;

  struct ar_hdr* p0 = ecget_ahdr(p, index);
  if (p0) {
    if (MODE_ISANY(o->action, OPTPROGRAM_VERBOSE)) {
      n += printf_text("ARCHIVE HEADER", USE_LT | USE_COLON | USE_EOL);
      n += printf_text("Name", USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_sore(p0->ar_name, sizeof(p0->ar_name), USE_STR | USE_EOL);

      n += printf_text("Date", USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_sore(p0->ar_date, sizeof(p0->ar_date), USE_STR | USE_EOL);

      n += printf_text("UID", USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_sore(p0->ar_uid, sizeof(p0->ar_uid), USE_STR | USE_EOL);

      n += printf_text("GID", USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_sore(p0->ar_gid, sizeof(p0->ar_gid), USE_STR | USE_EOL);

      n += printf_text("Mode", USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_sore(p0->ar_mode, sizeof(p0->ar_mode), USE_STR | USE_EOL);

      n += printf_text("Size", USE_TAB | USE_COLON | SET_PAD(MAXSIZE));
      n += printf_sore(p0->ar_size, sizeof(p0->ar_size), USE_STR | USE_EOL);
    }

    if (xstrneq(p0->ar_name, "/               ", sizeof(p0->ar_name))) {
      n += dump_achive0(p, o, index, MEMFIND_32BIT);
    } else if (xstrneq(p0->ar_name, "/SYM64/         ", sizeof(p0->ar_name))) {
      n += dump_achive0(p, o, index, MEMFIND_64BIT);
    } else if (xstrneq(p0->ar_name, "//              ", sizeof(p0->ar_name))) {
      n += dump_achive1(p, o, index, U32MASK_NONE);
    } else if ('/' == p0->ar_name[0] && isdec8(p0->ar_name[1])) {
      n += dump_achive2(p, o, index, U32MASK_NONE);
    } else if (isstr8(p0->ar_name[0])) {
      n += dump_achive3(p, o, index, U32MASK_NONE);
    } else {
      printf_w("unknown archive section '%s'", p0->ar_name);
    }
  }

  return n;
}

int readar(const pbuffer_t p, const poptions_t o) {
  if (isAR(p)) {
    int i = 0;
    while (ecget_ahdr(p, i)) {
      dump_archive(p, o, i);
      ++i;
    }
  } else {
    printf_e("not an archive file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

