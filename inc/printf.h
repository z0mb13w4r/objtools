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
#define USE_CHARCTRL      (20)
#define USE_TIMEDATE      (21)
#define USE_CORRUPT       (22)
#define USE_UNKNOWN       (23)

#define USE_HEXDUMP       (0)
#define USE_STRDUMP       (1)
#define USE_STR           (2)
#define USE_HEX           (3)

#define SET_FLAG(x)       ((x) << 16)

#define GET_FORMAT(x)     ((x) & (0x03 << 20))
#define SET_FORMAT(x)     ((x) << 20)
#define GET_BRACKET(x)    ((x) & (0x03 << 23))
#define SET_BRACKET(x)    ((x) << 23)
#define GET_POSTFIX(x)    ((x) & (0x03 << 26))
#define SET_POSTFIX(x)    ((x) << 26)
#define GET_PREFIX(x)     ((x) & (0x03 << 29))
#define SET_PREFIX(x)     ((x) << 29)

#define USE_EOL           SET_FLAG(1)

#define USE_NONE          SET_FORMAT(0)
#define USE_LT            SET_FORMAT(1)
#define USE_RT            SET_FORMAT(2)

#define USE_RB            SET_BRACKET(1)
#define USE_SB            SET_BRACKET(2)
#define USE_SQ            SET_BRACKET(3)
#define USE_DQ            SET_BRACKET(4)

#define USE_COLON         SET_POSTFIX(1)

#define USE_SPACE         SET_PREFIX(1)

#define USE_LTRB          (USE_LT | USE_RB)
#define USE_LTSB          (USE_LT | USE_SB)
#define USE_LTSQ          (USE_LT | USE_SQ)
#define USE_LTDQ          (USE_LT | USE_DQ)

int printf_eol();
int printf_nice(const uint64_t v, const int mode);
int printf_data(const void* p, const size_t size, const int addr, const int mode);
int printf_text(const char* p, const int mode);

int printf_mask(const pconvert_t p, const unsigned int mask);
int printf_masknone(const pconvert_t p, const unsigned int mask);

#endif

