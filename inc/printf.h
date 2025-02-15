#ifndef __PRINT_H_
#define __PRINT_H_

#include <stdint.h>

#include "common.h"
#include "objutils.h"

#define USE_DEC           (0)
#define USE_DEC2          (1)
#define USE_DEC5          (2)
#define USE_OCT           (3)
#define USE_OCT2          (4)
#define USE_OCT5          (5)
#define USE_FHEX          (6)
#define USE_LHEX          (7)
#define USE_FHEX8         (8)
#define USE_LHEX8         (9)
#define USE_FHEX16        (10)
#define USE_LHEX16        (11)
#define USE_FHEX24        (12)
#define USE_LHEX24        (13)
#define USE_FHEX32        (14)
#define USE_LHEX32        (15)
#define USE_FHEX48        (16)
#define USE_LHEX48        (17)
#define USE_FHEX64        (18)
#define USE_LHEX64        (19)
#define USE_TIMEDATE      (20)

#define USE_HEXDUMP       (0)
#define USE_STRDUMP       (1)
#define USE_STR           (2)
#define USE_HEX           (3)

int printf_nice(const uint64_t value, const int mode);
int printf_data(const void* data, const size_t size, const int addr, const int mode);

int printf_mask(const pconvert_t p, const unsigned int mask);
int printf_masknone(const pconvert_t p, const unsigned int mask);

#endif

