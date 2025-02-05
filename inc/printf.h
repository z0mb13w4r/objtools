#ifndef __PRINT_H_
#define __PRINT_H_

#include <stdint.h>

#include "common.h"

#define USE_DEC           (0)
#define USE_DEC2          (1)
#define USE_FHEX          (2)
#define USE_LHEX          (3)
#define USE_FHEX8         (4)
#define USE_LHEX8         (5)
#define USE_FHEX16        (6)
#define USE_LHEX16        (7)
#define USE_FHEX24        (8)
#define USE_LHEX24        (9)
#define USE_FHEX32        (10)
#define USE_LHEX32        (11)
#define USE_FHEX48        (12)
#define USE_LHEX48        (13)
#define USE_FHEX64        (14)
#define USE_LHEX64        (15)

#define USE_HEXDUMP       (0)
#define USE_STRDUMP       (1)
#define USE_STR           (2)
#define USE_HEX           (3)

int printf_nice(const uint64_t value, const int mode);
int printf_data(const void* data, const size_t size, const int addr, const int mode);

#endif

