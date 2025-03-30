#include "spin.h"

static int zap(int c0, int c1, int c2, int inc) {
  if (c0 >= c1 && c0 <= c2) {
    c0 += inc;
    if (c0 > c2) c0 = c1 + (c0 - c2) - 1;
  }

  return c0;
}

int rot5(const unknown_t p, const size_t size) {
  if (p) {
    char* p0 = CAST(char*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = zap(p0[i], '0', '9', 5);
    }

    return 0;
  }

  return -1;
}

int rot13(const unknown_t p, const size_t size) {
  if (p) {
    char *p0 = CAST(char*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = zap(p0[i], 'A', 'Z', 13);
      p0[i] = zap(p0[i], 'a', 'z', 13);
    }

    return 0;
  }

  return -1;
}

int rot18(const unknown_t p, const size_t size) {
  if (p) {
    char *p0 = CAST(char*, p);
    for (size_t i = 0; i < size; ++i) {
      p0[i] = zap(p0[i], 'A', 'Z', 13);
      p0[i] = zap(p0[i], 'a', 'z', 13);
      p0[i] = zap(p0[i], '0', '9', 5);
    }

    return 0;
  }

  return -1;
}

