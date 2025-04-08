#ifndef __DECODE_H_
#define __DECODE_H_

#include "defines.h"

int rot5(const unknown_t p, const size_t size);
int rot13(const unknown_t p, const size_t size);
int rot18(const unknown_t p, const size_t size);

int add8(const unknown_t p, const uchar_t v, const size_t size);
int sub8(const unknown_t p, const uchar_t v, const size_t size);
int xor8(const unknown_t p, const uchar_t v, const size_t size);

#endif

