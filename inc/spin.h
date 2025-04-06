#ifndef __SPIN_H_
#define __SPIN_H_

#include "defines.h"

int rot5(const unknown_t p, const size_t size);
int rot13(const unknown_t p, const size_t size);
int rot18(const unknown_t p, const size_t size);
int xor8(const unknown_t p, const uchar_t v, const size_t size);

#endif

