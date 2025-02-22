#ifndef __OBJUTILS_H_
#define __OBJUTILS_H_

#include "defines.h"

typedef struct convert_s {
  const char*        text;
  const unsigned int type;
} convert_t, *pconvert_t;

char* strname(char* dst, const char* src);

const char* strpick(const pconvert_t p, const pick_t x);
const char* strpicknull(const pconvert_t p, const pick_t x);
const char* strpickunknown(const pick_t x);

size_t      strlenpick(const pconvert_t p);

#endif

