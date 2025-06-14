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
#define USE_FHEX16        (17)
#define USE_LHEX16        (18)
#define USE_SHEX16        (19)
#define USE_FHEX24        (20)
#define USE_LHEX24        (21)
#define USE_FHEX32        (22)
#define USE_LHEX32        (23)
#define USE_SHEX32        (24)
#define USE_FHEX48        (25)
#define USE_LHEX48        (26)
#define USE_FHEX64        (27)
#define USE_LHEX64        (28)
#define USE_SHEX64        (29)
#define USE_POWER2        (30)
#define USE_PERCENT       (31)
#define USE_BOOL          (32)
#define USE_YESNO         (33)
#define USE_CHAR          (34)
#define USE_CHARCTRL      (35)
#define USE_TIMEDATE      (36)
#define USE_ERROR         (37)
#define USE_CORRUPT       (38)
#define USE_UNKNOWN       (39)
#define USE_WARNING       (40)

#define USE_REALp1        (0)
#define USE_REALp2        (1)
#define USE_REALp4        (2)
#define USE_REALp6        (3)

#define USE_HEXDUMP       (0)
#define USE_STRDUMP8      (1)
#define USE_STRDUMP16     (2)
#define USE_CODEDUMP      (3)
#define USE_STR           (4)
#define USE_STRSIZE       (5)
#define USE_STR16         (6)
#define USE_STR16SIZE     (7)
#define USE_HEX           (8)
#define USE_MD5           (9)
#define USE_SHA1          (10)
#define USE_SHA256        (11)
#define USE_SHA512        (12)
#define USE_HASHALL       (13)
#define USE_CRC8          (14)
#define USE_CRC16         (15)
#define USE_CRC32         (16)
#define USE_CRCALL        (17)
#define USE_ENTROPY       (18)
#define USE_ROT5          (19)
#define USE_ROT13         (20)
#define USE_ROT18         (21)
#define USE_BASE64        (22)
#define USE_GUID          (23)

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

#define USE_NONE          (0)

#define USE_LT            SET_FORMAT(1)
#define USE_RT            SET_FORMAT(2)
#define USE_FORMATMASK    SET_FORMAT(3)

#define USE_CB            SET_BRACKET(1)
#define USE_RB            SET_BRACKET(2)
#define USE_SB            SET_BRACKET(3)
#define USE_SBLT          SET_BRACKET(4)
#define USE_SBRT          SET_BRACKET(5)
#define USE_TB            SET_BRACKET(6)
#define USE_TBLT          SET_BRACKET(7)
#define USE_TBRT          SET_BRACKET(8)
#define USE_DRTB          SET_BRACKET(9)
#define USE_SQ            SET_BRACKET(10)
#define USE_DQ            SET_BRACKET(11)
#define USE_DQEQ          SET_BRACKET(12)
#define USE_PLUS          SET_BRACKET(13)
#define USE_DASH          SET_BRACKET(14)
#define USE_LINE          SET_BRACKET(15)
#define USE_OFFSET        SET_BRACKET(16)
#define USE_ADDRESS       SET_BRACKET(17)
#define USE_DISCRIMINATOR SET_BRACKET(18)
#define USE_BRACKETMASK   SET_BRACKET(63)

#define USE_SPACE         SET_POS0(1)
#define USE_TAB           SET_POS0(2)
#define USE_TAB2          SET_POS0(3)
#define USE_AT            SET_POS0(4)
#define USE_ATAT          SET_POS0(5)
#define USE_DOT           SET_POS0(6)
#define USE_COMMA         SET_POS0(7)
#define USE_0x            SET_POS0(8)
#define USE_POS0MASK      SET_POS0(15)

#define USE_COLON         SET_POS1(1)
#define USE_SEMICOLON     SET_POS1(2)
#define USE_BYTES         SET_POS1(3)
#define USE_COUNT         SET_POS1(4)
#define USE_MIN           SET_POS1(5)
#define USE_MAX           SET_POS1(6)
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
#define USE_NOTEXT        SET_FLAG(2)
#define USE_NOSPACE       SET_FLAG(4)
#define USE_FLAGMASK      SET_FLAG(15)

int printf_eol();

int printf_spos(char* o, const size_t size, const imode_t mode, const bool_t usespace);
int printf_epos(char* o, const size_t size, const imode_t mode);

int printf_color(const imode_t mode);

int printf_work(char* o, const size_t size, const char* p, const imode_t mode);
int printf_join(const char* p, const uint64_t v, const imode_t mode);
int printf_yoke(const char* p, const char* q, const imode_t mode);
int printf_book(const char* p[], const imode_t mode);
int printf_text(const char* p, const imode_t mode);

int printf_neat(char* o, const size_t size, const uint64_t v, const imode_t mode);
int printf_nice(const uint64_t v, const imode_t mode);

int printf_tidy(char* o, const size_t size, const double v, const imode_t mode);
int printf_real(const double v, const imode_t mode);

int printf_data(const unknown_t p, const size_t size, const addrz_t addr, const imode_t mode);
int printf_hurt(const unknown_t p, const size_t size, const imode_t mode);
int printf_sore(const unknown_t p, const size_t size, const imode_t mode);

int printf_mask(const pconvert_t p, const maskz_t mask, const imode_t mode);
int printf_maskmute(const pconvert_t p, const maskz_t mask, const imode_t mode);
int printf_masknone(const pconvert_t p, const maskz_t mask, const imode_t mode);

int printf_pick(const pconvert_t p, const pick_t x, const imode_t mode);
int printf_pack(const int size);

int printf_errname(const char* name);

int  printf_d(const char* format, ...);
int  printf_e(const char* format, ...);
int  printf_i(const char* format, ...);
int  printf_w(const char* format, ...);
void printf_x(const char* format, ...);

#endif

