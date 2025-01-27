#ifndef __PRINT_H_
#define __PRINT_H_

#define USE_DECIMAL       (0)
#define USE_DECIMAL2      (1)
#define USE_FHEX8         (2)
#define USE_LHEX8         (3)
#define USE_FHEX16        (4)
#define USE_LHEX16        (5)
#define USE_FHEX32        (6)
#define USE_LHEX32        (7)
#define USE_FHEX64        (8)
#define USE_LHEX64        (9)

int printf_nice(const uint64_t value, const int mode);

#endif

