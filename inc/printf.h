#ifndef __PRINT_H_
#define __PRINT_H_

#define USE_DEC           (0)
#define USE_DEC2          (1)
#define USE_FHEX          (2)
#define USE_LHEX          (3)
#define USE_FHEX8         (4)
#define USE_LHEX8         (5)
#define USE_FHEX16        (6)
#define USE_LHEX16        (7)
#define USE_FHEX32        (8)
#define USE_LHEX32        (9)
#define USE_FHEX64        (10)
#define USE_LHEX64        (11)

int printf_nice(const uint64_t value, const int mode);

#endif

