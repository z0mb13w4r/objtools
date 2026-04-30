#ifndef __PRINT_CUSTOM_H_
#define __PRINT_CUSTOM_H_

#include "printf.h"
#include "options.h"

int printf_fork(const poptions_t o, const pconvert_t p, const pconvert_t q, const pick_t x, const imode_t mode);
int printf_size(const poptions_t o, const uint64_t v, const imode_t mode);

#endif

