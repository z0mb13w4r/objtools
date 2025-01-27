#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

#include "printf.h"

int printf_nice(const uint64_t value, const int mode) {
  switch (mode) {
  case USE_DECIMAL:                 return printf(" %" PRId64, value);
  case USE_DECIMAL2:                return printf(" %2" PRId64, value);
  case USE_FHEX8:                   return printf(" 0x%2.2" PRIx64, value);
  case USE_LHEX8:                   return printf(" %2.2" PRIx64, value);
  case USE_FHEX16:                  return printf(" 0x%4.4" PRIx64, value);
  case USE_LHEX16:                  return printf(" %4.4" PRIx64, value);
  case USE_FHEX32:                  return printf(" 0x%8.8" PRIx64, value);
  case USE_LHEX32:                  return printf(" %8.8" PRIx64, value);
  case USE_FHEX64:                  return printf(" 0x%16.16" PRIx64, value);
  case USE_LHEX64:                  return printf(" %16.16" PRIx64, value);
  default:
    break;
  }

  return 0;
}

