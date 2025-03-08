#ifndef __DEFINES_H_
#define __DEFINES_H_

#include <limits.h>
#include <stdint.h>
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

#define MODE_MASK0              (0x00FFFFFF)
#define MODE_MASK1              (0xFF000000)
#define MODE_MASK               (MODE_MASK0 | MODE_MASK1)

#define MODE_GET0(x)            (((x) >>  0) & 0xFF)
#define MODE_GET1(x)            (((x) >>  8) & 0xFF)
#define MODE_GET2(x)            (((x) >> 16) & 0xFF)
#define MODE_GET3(x)            (((x) >> 24) & 0xFF)

#define MODE_PUT0(x)            ((x) <<  0)
#define MODE_PUT1(x)            ((x) <<  8)
#define MODE_PUT2(x)            ((x) << 16)
#define MODE_PUT3(x)            ((x) << 24)

typedef uint64_t addrz_t;
typedef uint64_t maskz_t;
typedef uint64_t imode_t;
typedef char     smode_t[4];

typedef void*    handle_t;
typedef void*    unknown_t;

typedef uint32_t pick_t;

#endif

