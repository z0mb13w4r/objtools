#ifndef __OBJUTILS_H_
#define __OBJUTILS_H_

#include "crc.h"
#include "hash.h"

#define PICK_END               (~0)

typedef struct convert_s {
  const char*  text;
  const pick_t type;
} convert_t, *pconvert_t;

bool_t isbits(const imode_t x);
bool_t isused(pick_t* p, const pick_t x);

uint64_t ulog2(uint64_t  x);

char* strname(char* dst, const char* src);

const char* strpick(const pconvert_t p, const pick_t x);
const char* strpicknull(const pconvert_t p, const pick_t x);
const char* strpickunknown(const pick_t x);

size_t strlenpick(const pconvert_t p);

size_t fsize(FILE *f);

int crc32(const unknown_t p, const size_t size, pulong_t crc);

#endif

