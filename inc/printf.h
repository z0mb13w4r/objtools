#ifndef __PRINT_H_
#define __PRINT_H_

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
#define USE_HEX2          (10)
#define USE_HEX4          (11)
#define USE_FHEX          (12)
#define USE_LHEX          (13)
#define USE_FHEX8         (14)
#define USE_LHEX8         (15)
#define USE_SHEX8         (16)
#define USE_LHEX8NS       (17)
#define USE_FHEX16        (18)
#define USE_LHEX16        (19)
#define USE_SHEX16        (20)
#define USE_FHEX24        (21)
#define USE_LHEX24        (22)
#define USE_FHEX32        (23)
#define USE_LHEX32        (24)
#define USE_SHEX32        (25)
#define USE_FHEX48        (26)
#define USE_LHEX48        (27)
#define USE_FHEX64        (28)
#define USE_LHEX64        (29)
#define USE_SHEX64        (30)
#define USE_POWER2        (31)
#define USE_PERCENT       (32)
#define USE_CHAR          (33)
#define USE_CHARCTRL      (34)
#define USE_TIMEDATE      (35)
#define USE_ERROR         (36)
#define USE_CORRUPT       (37)
#define USE_UNKNOWN       (38)
#define USE_WARNING       (39)

#define USE_HEXDUMP       (0)
#define USE_STRDUMP       (1)
#define USE_STR           (2)
#define USE_HEX           (3)
#define USE_MD5           (4)
#define USE_SHA1          (5)
#define USE_SHA256        (6)
#define USE_SHA512        (7)
#define USE_HASHALL       (8)
#define USE_CRC32         (9)
#define USE_BASE64        (10)
#define USE_GUID          (11)

#define GET_XX2(x,y)      ((x) & (0x03 << (y)))
#define SET_XX2(x,y)      (((x) & 0x03) << (y))
#define GET_XX3(x,y)      ((x) & (0x07 << (y)))
#define SET_XX3(x,y)      (((x) & 0x07) << (y))
#define GET_XX4(x,y)      ((x) & (0x0f << (y)))
#define SET_XX4(x,y)      (((x) & 0x0f) << (y))
#define GET_XX6(x,y)      ((x) & (0x3f << (y)))
#define SET_XX6(x,y)      (((x) & 0x3f) << (y))
#define GET_XX8(x,y)      ((x) & (0xff << (y)))
#define SET_XX8(x,y)      (((x) & 0xff) << (y))

#define GET_PAD(x)        GET_XX8(x, 0)
#define SET_PAD(x)        SET_XX8(x, 0)
#define GET_FORMAT(x)     GET_XX2(x, 8)
#define SET_FORMAT(x)     SET_XX2(x, 8)
#define GET_BRACKET(x)    GET_XX6(x, 10)
#define SET_BRACKET(x)    SET_XX6(x, 10)
#define GET_POS0(x)       GET_XX4(x, 16)
#define SET_POS0(x)       SET_XX4(x, 16)
#define GET_POS1(x)       GET_XX4(x, 20)
#define SET_POS1(x)       SET_XX4(x, 20)
#define GET_COLOR(x)      GET_XX4(x, 24)
#define SET_COLOR(x)      SET_XX4(x, 24)
#define SET_FLAG(x)       SET_XX4(x, 28)

#define USE_LT            SET_FORMAT(1)
#define USE_RT            SET_FORMAT(2)
#define USE_FORMATMASK    SET_FORMAT(3)

#define USE_CB            SET_BRACKET(1)
#define USE_RB            SET_BRACKET(2)
#define USE_SB            SET_BRACKET(3)
#define USE_SBLT          SET_BRACKET(4)
#define USE_SBRT          SET_BRACKET(5)
#define USE_TB            SET_BRACKET(6)
#define USE_DRTB          SET_BRACKET(7)
#define USE_SQ            SET_BRACKET(8)
#define USE_DQ            SET_BRACKET(9)
#define USE_DQEQ          SET_BRACKET(10)
#define USE_PLUS          SET_BRACKET(11)
#define USE_DASH          SET_BRACKET(12)
#define USE_BRACKETMASK   SET_BRACKET(63)

#define USE_SPACE         SET_POS0(1)
#define USE_TAB           SET_POS0(2)
#define USE_TAB2          SET_POS0(3)
#define USE_AT            SET_POS0(4)
#define USE_ATAT          SET_POS0(5)
#define USE_DOT           SET_POS0(6)
#define USE_POS0MASK      SET_POS0(15)

#define USE_COLON         SET_POS1(1)
#define USE_BYTES         SET_POS1(2)
#define USE_POS1MASK      SET_POS1(15)

#define USE_RESET         SET_COLOR(1)
#define USE_BLACK         SET_COLOR(2)
#define USE_RED           SET_COLOR(3)
#define USE_GREEN         SET_COLOR(4)
#define USE_YELLOW        SET_COLOR(5)
#define USE_BLUE          SET_COLOR(6)
#define USE_PURPLE        SET_COLOR(7)
#define USE_CYAN          SET_COLOR(8)
#define USE_WHITE         SET_COLOR(9)
#define USE_COLORMASK     SET_COLOR(15)

#define USE_EOL           SET_FLAG(1)
#define USE_NOSPACE       SET_FLAG(2)
#define USE_FLAGMASK      SET_FLAG(15)

int printf_eol();

int printf_spos(char* o, const size_t size, const imode_t mode, const bool_t usespace);
int printf_epos(char* o, const size_t size, const imode_t mode);

int printf_color(const imode_t mode);

int printf_work(char* o, const size_t size, const char* p, const imode_t mode);
int printf_book(const char* p[], const imode_t mode);
int printf_text(const char* p, const imode_t mode);

int printf_neat(char* o, const size_t size, const uint64_t v, const imode_t mode);
int printf_nice(const uint64_t v, const imode_t mode);

int printf_data(const void* p, const size_t size, const addrz_t addr, const imode_t mode);
int printf_sore(const void* p, const size_t size, const imode_t mode);

int printf_mask(const pconvert_t p, const maskz_t mask, const imode_t mode);
int printf_maskmute(const pconvert_t p, const maskz_t mask, const imode_t mode);
int printf_masknone(const pconvert_t p, const maskz_t mask, const imode_t mode);

int printf_pick(const pconvert_t p, const pick_t x, const imode_t mode);
int printf_pack(const int size);

void set_errname(const char* name);

int  printf_d(const char* format, ...);
int  printf_e(const char* format, ...);
int  printf_i(const char* format, ...);
int  printf_w(const char* format, ...);
void printf_x(const char* format, ...);

#endif

