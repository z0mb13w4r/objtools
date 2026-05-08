#ifndef __DEFINES_H_
#define __DEFINES_H_

#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//#define BUILD_UBUNTU_20_04
//#define BUILD_UBUNTU_24_04

#define ECODE_OK             (0)
#define ECODE_GENERIC        (-1)
#define ECODE_MALLOC         (-2)
#define ECODE_DWARF          (-3)
#define ECODE_CRYPTO         (-4)
#define ECODE_CAPSTONE       (-5)
#define ECODE_OPCODE         (-6)
#define ECODE_BFD            (-7)
#define ECODE_PARAM          (-8)
#define ECODE_ARGUMENTS      (-9)
#define ECODE_CORRUPT        (-10)
#define ECODE_HANDLE         (-298)
#define ECODE_NULL           (-299)
#define ECODE_NOENTRY        (-300)

#define ECODE_ISOK(x)        (ECODE_OK <= (x))
#define ECODE_ISEVIL(x)      (ECODE_OK >  (x))
#define ECODE_ISFAILED(x)    (ECODE_ISEVIL(x) && !ECODE_ISWARNING(x))
#define ECODE_ISNOENTRY(x)   (ECODE_NOENTRY == (x))
#define ECODE_ISWARNING(x)   (ECODE_NOENTRY <= (x))

#ifndef CAST
#define CAST(x,y)    ((x)(y))
#endif

#ifndef MALLOCX
#define MALLOCX(x,y) CAST(x, mallocx(y))
#endif

#ifndef MALLOCA
#define MALLOCA(x,y,z) x y[z]; xmemset(y, 0, sizeof(y))
#define STATICA(x,y,z) static MALLOCA(x,y,z)
#endif

#ifndef MALLOCACOPY
#define MALLOCACOPY(x,y,z,s) MALLOCA(x,y,z); xstrncpy(y, s, sizeof(y))
#endif

#ifndef MALLOCS
#define MALLOCS(x,y) x y; x* p##y = &y; xmemset(p##y, 0, sizeof(y))
#endif

#ifndef MALLOCSMODE
#define MALLOCSMODE(x,y,z) MALLOCS(x,y); setmode(p##y, z)
#endif

#ifndef NELEMENTS
#define NELEMENTS(x) (sizeof(x) / sizeof(x[0]))
#endif

#ifndef MEMCPYA
#define MEMCPYA(x,y) xmemcpy(x, y, MIN(NELEMENTS(x), NELEMENTS(y)))
#endif

#ifndef ABS
#define ABS(x)       (((x) < 0) ? -(x) : (x))
#endif

#ifndef SIGN
#define SIGN(x)      (((x) < 0) ? -1 : 1)
#endif

#ifndef SIGNF
#define SIGNF(x)     (((x) < 0) ? -1.0f : 1.0f)
#endif

#ifndef MIN
#define MIN(x,y)     (((x) > (y)) ? (y) : (x))
#endif

#ifndef MAX
#define MAX(x,y)     (((x) < (y)) ? (y) : (x))
#endif

#ifndef MID
#define MID(x,y,z)   MAX(x, MIN(y, z))
#endif

#ifndef LIMIT
#define LIMIT(x,b,t) MAX(b, MIN(x, t))
#endif

#ifndef BOUND16
#define BOUND16(x)   (CAST(size_t, ((x) + 1) / 2) * 2)
#endif

#ifndef BOUND32
#define BOUND32(x)   (CAST(size_t, ((x) + 3) / 4) * 4)
#endif

#ifndef U32MASK
#define U32MASK_NONE (0)
#define U32MASK(x)   (1 << (x))
#endif

#ifndef U64MASK
#define U64MASK_NONE (0)
#define U64MASK(x)   (1ULL << (x))
#endif

#ifndef TRUE
#define TRUE         (1)
#endif

#ifndef FALSE
#define FALSE        (0)
#endif

#ifndef STRING
#define STRING(x)    ((x) ? CAST(char*, x) : "")
#endif

#ifndef YESNO
#define YESNO(x)     ((x) ? "y" : "n")
#endif

#define MODE_GET0(x)            MODE_MASK8((x) >>  0)
#define MODE_GET1(x)            MODE_MASK8((x) >>  8)
#define MODE_GET2(x)            MODE_MASK8((x) >> 16)
#define MODE_GET3(x)            MODE_MASK8((x) >> 24)
#define MODE_GET4(x)            MODE_MASK8((x) >> 32)
#define MODE_GET5(x)            MODE_MASK8((x) >> 40)
#define MODE_GET6(x)            MODE_MASK8((x) >> 48)
#define MODE_GET7(x)            MODE_MASK8((x) >> 56)

#define MODE_PUT0(x)            (MODE_MASK8(x) <<  0)
#define MODE_PUT1(x)            (MODE_MASK8(x) <<  8)
#define MODE_PUT2(x)            (MODE_MASK8(x) << 16)
#define MODE_PUT3(x)            (MODE_MASK8(x) << 24)
#define MODE_PUT4(x)            (MODE_MASK8(x) << 32)
#define MODE_PUT5(x)            (MODE_MASK8(x) << 40)
#define MODE_PUT6(x)            (MODE_MASK8(x) << 48)
#define MODE_PUT7(x)            (MODE_MASK8(x) << 56)

#define MODE_MAKE(x,y,z)        (MODE_PUT0(x) | MODE_PUT1(y) | MODE_PUT2(z))

#define MODE_HIDE8(x)           ((x) & ~0xff)
#define MODE_HIDE12(x)          ((x) & ~0xfff)
#define MODE_HIDE16(x)          ((x) & ~0xffff)
#define MODE_HIDE24(x)          ((x) & ~0xffffff)
#define MODE_HIDE32(x)          ((x) & ~0xffffffff)

#define MODE_MASK0              (0x00ffffff)
#define MODE_MASK1              (0xff000000)
#define MODE_MASK2(x)           ((x) & 0x03)
#define MODE_MASK3(x)           ((x) & 0x07)
#define MODE_MASK4(x)           ((x) & 0x0f)
#define MODE_MASK4LO(x)         ((x) & 0x0f)
#define MODE_MASK4HI(x)         ((x) & 0xf0)
#define MODE_MASK6(x)           ((x) & 0x3f)
#define MODE_MASK8(x)           ((x) & 0xff)
#define MODE_MASK8LO(x)         ((x) & 0x00ff)
#define MODE_MASK8HI(x)         ((x) & 0xff00)
#define MODE_MASK12(x)           ((x) & 0xfff)
#define MODE_MASK16(x)          ((x) & 0xffff)
#define MODE_MASK16LO(x)        ((x) & 0x0000ffff)
#define MODE_MASK16HI(x)        ((x) & 0xffff0000)
#define MODE_MASK24(x)          ((x) & 0xffffff)
#define MODE_MASK32(x)          ((x) & 0xffffffff)
#define MODE_MASK               (MODE_MASK0 | MODE_MASK1)

#define MODE_ISANY(x,y)         ((x) & (y))
#define MODE_ISSET(x,y)         (((x) & (y)) == (y))
#define MODE_ISNOT(x,y)         (0 == MODE_ISANY(x, y))
#define MODE_ISFIX(x,y,z)       (MODE_ISSET(x, y) && MODE_ISNOT(x, z))

#define MODE_ISLOCKED(x,y)      ((x) == (y))
#define MODE_ISLOCKED8(x,y)     (MODE_MASK8(x)  == MODE_MASK8(y))
#define MODE_ISLOCKED16(x,y)    (MODE_MASK16(x) == MODE_MASK16(y))
#define MODE_ISLOCKED24(x,y)    (MODE_MASK24(x) == MODE_MASK24(y))
#define MODE_ISLOCKED32(x,y)    (MODE_MASK32(x) == MODE_MASK32(y))

#define GET_XX2(x,y)            ((x) & (0x03 << (y)))
#define SET_XX2(x,y)            (MODE_MASK2(x) << (y))
#define GET_XX3(x,y)            ((x) & (0x07 << (y)))
#define SET_XX3(x,y)            (MODE_MASK3(x) << (y))
#define GET_XX4(x,y)            ((x) & (0x0f << (y)))
#define SET_XX4(x,y)            (MODE_MASK4(x) << (y))
#define GET_XX6(x,y)            ((x) & (0x3fULL << (y)))
#define SET_XX6(x,y)            (MODE_MASK6(x) << (y))
#define GET_XX8(x,y)            ((x) & (0xff << (y)))
#define SET_XX8(x,y)            (MODE_MASK8(x) << (y))

typedef uint64_t addrz_t;
typedef uint64_t maskz_t;
typedef uint64_t imode_t;

typedef uint32_t nmode_t;
typedef char     smode_t[4];

typedef void    *handle_t;
typedef void    *unknown_t, **punknown_t;

typedef const void *cunknown_t;

typedef uint64_t pick_t, *ppick_t;

typedef int      bool_t;

typedef unsigned int   uint_t,   *puint_t;
typedef unsigned char  uchar_t,  *puchar_t;
typedef unsigned long  ulong_t,  *pulong_t;
typedef unsigned short ushort_t, *pushort_t;

typedef char           CHAR, *PCHAR;
typedef int32_t        LONG, *PLONG;
typedef uint32_t       ULONG, *PULONG;
typedef uint16_t       WCHAR, *PWCHAR;

typedef uint8_t        BYTE, *PBYTE;
typedef uint16_t       WORD, *PWORD;
typedef uint32_t       DWORD, *PDWORD;
typedef uint64_t       ULONGLONG, *PULONGULONG;

#endif

