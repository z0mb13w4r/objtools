#include "arcode.h"
#include "printf.h"
#include "readar.h"
#include "objutils.h"

static int dump_archeader(const pbuffer_t p, const poptions_t o) {
  const int MAXSIZE = 36;

  int n = 0;

  struct ar_hdr* p0 = ecget_ahdr(p);
  if (p0) {
    n += printf_text("ARCHIVE HEADER", USE_LT | USE_COLON | USE_EOL);
    n += printf_text("Name", USE_TAB | USE_COLON | SET_PAD(MAXSIZE - 1));
    n += printf_text(p0->ar_name, USE_LT | USE_SPACE | USE_EOL);

  }

  return n;
}

int readar(const pbuffer_t p, const poptions_t o) {
  if (isAR(p)) {
    dump_archeader(p, o);
  } else {
    printf_e("not an archive file - it has the wrong magic bytes at the start.");
  }

  return 0;
}

