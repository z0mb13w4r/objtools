#ifndef __PRINT_H_
#define __PRINT_H_

#include "common.h"
#include "objutils.h"

#define USE_DEC           (0)
#define USE_DEC2          (1)
#define USE_DEC2Z         (2)
#define USE_DEC3          (3)
#define USE_DEC3Z         (4)
#define USE_DEC5          (5)
#define USE_DEC5Z         (6)
#define USE_OCT           (7)
#define USE_OCT2          (8)
#define USE_OCT5          (9)
#define USE_FHEX          (10)
#define USE_LHEX          (11)
#define USE_FHEX8         (12)
#define USE_LHEX8         (13)
#define USE_LHEX8NS       (14)
#define USE_FHEX16        (15)
#define USE_LHEX16        (16)
#define USE_FHEX24        (17)
#define USE_LHEX24        (18)
#define USE_FHEX32        (19)
#define USE_LHEX32        (20)
#define USE_FHEX48        (21)
#define USE_LHEX48        (22)
#define USE_FHEX64        (23)
#define USE_LHEX64        (24)
#define USE_PERCENT       (25)
#define USE_CHAR          (26)
#define USE_CHARCTRL      (27)
#define USE_TIMEDATE      (28)
#define USE_ERROR         (29)
#define USE_CORRUPT       (30)
#define USE_UNKNOWN       (31)
#define USE_WARNING       (32)

#define USE_HEXDUMP       (0)
#define USE_STRDUMP       (1)
#define USE_STR           (2)
#define USE_HEX           (3)

#define GET_XX3(x,y)      ((x) & (0x07 << (y)))
#define SET_XX3(x,y)      (((x) & 0x07) << (y))
#define GET_XX4(x,y)      ((x) & (0x0f << (y)))
#define SET_XX4(x,y)      (((x) & 0x0f) << (y))
#define GET_XX8(x,y)      ((x) & (0xff << (y)))
#define SET_XX8(x,y)      (((x) & 0xff) << (y))

#define GET_PAD(x)        GET_XX8(x, 0)
#define SET_PAD(x)        SET_XX8(x, 0)
#define GET_FORMAT(x)     GET_XX4(x, 8)
#define SET_FORMAT(x)     SET_XX4(x, 8)
#define GET_BRACKET(x)    GET_XX4(x, 12)
#define SET_BRACKET(x)    SET_XX4(x, 12)
#define GET_POS1(x)       GET_XX4(x, 16)
#define SET_POS1(x)       SET_XX4(x, 16)
#define GET_POS0(x)       GET_XX4(x, 20)
#define SET_POS0(x)       SET_XX4(x, 20)

#define SET_FLAG(x)       SET_XX4(x, 28)

#define USE_EOL           SET_FLAG(1)
#define USE_FLAGMASK      SET_FLAG(15)

#define USE_LT            SET_FORMAT(1)
#define USE_RT            SET_FORMAT(2)

#define USE_CB            SET_BRACKET(1)
#define USE_RB            SET_BRACKET(2)
#define USE_SB            SET_BRACKET(3)
#define USE_TB            SET_BRACKET(4)
#define USE_DRTB          SET_BRACKET(5)
#define USE_SQ            SET_BRACKET(6)
#define USE_DQ            SET_BRACKET(7)
#define USE_BRACKETMASK   SET_BRACKET(15)

#define USE_COLON         SET_POS1(1)
#define USE_BYTES         SET_POS1(2)
#define USE_POS1MASK      SET_POS1(15)

#define USE_SPACE         SET_POS0(1)
#define USE_TAB           SET_POS0(2)
#define USE_AT            SET_POS0(3)
#define USE_POS0MASK      SET_POS0(15)

int printf_eol();
int printf_nice(const uint64_t v, const modez_t mode);
int printf_data(const void* p, const size_t size, const addrz_t addr, const modez_t mode);
int printf_text(const char* p, const modez_t mode);

int printf_mask(const pconvert_t p, const maskz_t mask, const modez_t mode);
int printf_maskmute(const pconvert_t p, const maskz_t mask, const modez_t mode);
int printf_masknone(const pconvert_t p, const maskz_t mask, const modez_t mode);

int printf_pick(const pconvert_t p, const pick_t x, const modez_t mode);
int printf_pack(const size_t size);

#endif

