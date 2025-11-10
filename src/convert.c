#include "decode.h"
#include "printf.h"
#include "convert.h"

int dump_shlbrute8(const handle_t o, const unknown_t p, const size_t size, const int32_t step) {
  int n = 0;

  return n;
}

int dump_shlbrute16(const handle_t o, const unknown_t p, const size_t size, const int32_t step) {
  int n = 0;

  return n;
}

int dump_shrbrute8(const handle_t o, const unknown_t p, const size_t size, const int32_t step) {
  int n = 0;

  return n;
}

int dump_shrbrute16(const handle_t o, const unknown_t p, const size_t size, const int32_t step) {
  int n = 0;

  return n;
}

int dump_xorbrute8(const handle_t o, const unknown_t p, const size_t size, const int32_t step) {
  int n = 0;

  if (p && size) {
    for (int i = 1; i < 256; ++i) {
      unknown_t p0 = cmalloc(p, size);
      if (p0) {
        if (ECODE_ISOK(xor8(p0, i, size, step))) {
          n += printf_text("Key =", USE_LT);
          n += printf_nice(i, USE_LHEX8 | USE_COLON);
          n += printf_sore(p0, size, USE_STR | USE_SPACE);
          n += printf_eol();
        }
      }
    }
  }

  return n;
}

int dump_xorbrute16(const handle_t o, const unknown_t p, const size_t size, const int32_t step) {
  int n = 0;

  return n;
}

