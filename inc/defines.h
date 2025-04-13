#ifndef __DEFINES_H_
#define __DEFINES_H_

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef CAST
#define CAST(x,y)    ((x)(y))
#endif

#ifndef MALLOCA
#define MALLOCA(x,y,z) x y[z]; memset(y, 0, sizeof(y))
#endif

#ifndef MALLOCS
#define MALLOCS(x,y) x y; x* p##y = &y; memset(p##y, 0, sizeof(y))
#endif

#ifndef MALLOCSMODE
#define MALLOCSMODE(x,y,z) x y; x* p##y = &y; memset(p##y, 0, sizeof(y)); setmode(p##y, z)
#endif

#ifndef MALLOCX
#define MALLOCX(x,y) CAST(x, mallocx(y))
#endif

#ifndef NELEMENTS
#define NELEMENTS(x) (sizeof(x) / sizeof(x[0]))
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

#ifndef U64MASK
#define U64MASK(x)   (1ULL << (x))
#endif

#ifndef TRUE
#define TRUE         (1)
#endif

#ifndef FALSE
#define FALSE        (0)
#endif

#define MODE_GET0(x)            (((x) >>  0) & 0xff)
#define MODE_GET1(x)            (((x) >>  8) & 0xff)
#define MODE_GET2(x)            (((x) >> 16) & 0xff)
#define MODE_GET3(x)            (((x) >> 24) & 0xff)

#define MODE_PUT0(x)            ((x) <<  0)
#define MODE_PUT1(x)            ((x) <<  8)
#define MODE_PUT2(x)            ((x) << 16)
#define MODE_PUT3(x)            ((x) << 24)

#define MODE_MAKE(x0,x1,x2)     (MODE_PUT0(x0) | MODE_PUT1(x1) | MODE_PUT2(x2))

#define MODE_MASK0              (0x00ffffff)
#define MODE_MASK1              (0xff000000)
#define MODE_MASK               (MODE_MASK0 | MODE_MASK1)

typedef uint64_t addrz_t;
typedef uint64_t maskz_t;
typedef uint64_t imode_t;

typedef uint32_t nmode_t;
typedef char     smode_t[4];

typedef void*    handle_t;
typedef void*    unknown_t;

typedef uint32_t pick_t, *ppick_t;

typedef int      bool_t;

typedef unsigned int   uint_t;
typedef unsigned char  uchar_t, *puchar_t;
typedef unsigned long  ulong_t, *pulong_t;
typedef unsigned short ushort_t, *pushort_t;

typedef char           CHAR, *PCHAR;
typedef int32_t        LONG, *PLONG;
typedef uint32_t       ULONG, *PULONG;
typedef wchar_t        WCHAR;

typedef uint8_t        BYTE, *PBYTE;
typedef uint16_t       WORD, *PWORD;
typedef uint32_t       DWORD, *PDWORD;
typedef uint64_t       ULONGLONG, *PULONGULONG;

#endif

