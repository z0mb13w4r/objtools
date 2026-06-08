#include "arcode.h"
#include "printf.h"
#include "readar.h"
#include "objutils.h"

static int dump_archive(const pbuffer_t p, const poptions_t o, const int index) {
  const int MAXSIZE = 36;

  int n = 0;

  struct ar_hdr* p0 = ecget_ahdr(p, index);
  if (p0) {
    n += printf_text("ARCHIVE HEADER", USE_LT | USE_COLON | USE_EOL);
    n += printf_text("Name", USE_TAB | USE_COLON | SET_PAD(MAXSIZE - 1));
    n += printf_sore(p0->ar_name, sizeof(p0->ar_name), USE_STR | USE_EOL);

    n += printf_text("Date", USE_TAB | USE_COLON | SET_PAD(MAXSIZE - 1));
    n += printf_sore(p0->ar_date, sizeof(p0->ar_date), USE_STR | USE_EOL);

    n += printf_text("UID", USE_TAB | USE_COLON | SET_PAD(MAXSIZE - 1));
    n += printf_sore(p0->ar_uid, sizeof(p0->ar_uid), USE_STR | USE_EOL);

    n += printf_text("GID", USE_TAB | USE_COLON | SET_PAD(MAXSIZE - 1));
    n += printf_sore(p0->ar_gid, sizeof(p0->ar_gid), USE_STR | USE_EOL);

    n += printf_text("Mode", USE_TAB | USE_COLON | SET_PAD(MAXSIZE - 1));
    n += printf_sore(p0->ar_mode, sizeof(p0->ar_mode), USE_STR | USE_EOL);

    n += printf_text("Size", USE_TAB | USE_COLON | SET_PAD(MAXSIZE - 1));
    n += printf_sore(p0->ar_size, sizeof(p0->ar_size), USE_STR | USE_EOL);

printf("size = %ld\n", decb(p0->ar_size, sizeof(p0->ar_size)));

    if (xstrncmp(p0->ar_name, "/               ", sizeof(p0->ar_name))) {
    } else if (xstrncmp(p0->ar_name, "/SYM64/         ", sizeof(p0->ar_name))) {
    } else if (xstrncmp(p0->ar_name, "//              ", sizeof(p0->ar_name))) {
    }
  }

  return n;
}

int readar(const pbuffer_t p, const poptions_t o) {
  if (isAR(p)) {
    dump_archive(p, o, 0);
    dump_archive(p, o, 1);
  } else {
    printf_e("not an archive file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

