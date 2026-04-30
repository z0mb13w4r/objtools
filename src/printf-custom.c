#include "printf-custom.h"

int printf_fork(const poptions_t o, const pconvert_t p, const pconvert_t q, const pick_t x, const imode_t mode) {
  int n = 0;

  n += printf_nice(x, mode & ~USE_FLAGMASK);
  if (MODE_ISANY(o->action, OPTPROGRAM_VERBOSE)) {
    n += printf_pick(p, x, USE_SPACE | GET_FLAG(mode));
  } else {
    n += printf_pick(q, x, USE_SPACE | GET_FLAG(mode));
  }

  return n;
}

int printf_size(const poptions_t o, const uint64_t v, const imode_t mode) {
  int n = 0;

  if (MODE_ISANY(o->action, OPTPROGRAM_VERBOSE)) {
    n += printf_nice(v, USE_BYTES);
    n += printf_nice(v, USE_FHEX | GET_FLAG(mode));
  } else {
    n += printf_nice(v, mode);
  }

  return n;
}

