#ifndef __OBJUTILS_H_
#define __OBJUTILS_H_

#include "crc.h"
#include "hash.h"
#include "memuse.h"

#define PICK_END               (~0)

typedef struct args_s {
  char    option1;
  char   *option2;
  imode_t action;
  char   *content;
  imode_t groups;
} args_t, *pargs_t;

typedef struct convert_s {
  const char*  text;
  const pick_t type;
  union {
    unknown_t param;
    uint64_t  value;
  };
} convert_t, *pconvert_t;

int countbits(const imode_t x);

bool_t isbits(const imode_t x);
bool_t isused(ppick_t p, const pick_t x);
bool_t isnamedone(const char* names[], const size_t maxnames, const char* check);

bool_t isdec8(int x);
bool_t isdecb(unknown_t p, const size_t size);

bool_t ishex8(int x);
bool_t ishexb(unknown_t p, const size_t size);

uint64_t ulog2(uint64_t  x);
uint64_t atoimode(const char* src);
uint64_t atovalue(const char* src);

char* strname(char* dst, const char* src);

pconvert_t convertpicknull(const pconvert_t p, const pick_t x);

const char* strpick(const pconvert_t p, const pick_t x);
const char* strpicknull(const pconvert_t p, const pick_t x);
const char* strpickunknown(const pick_t x);

size_t strlenpick(const pconvert_t p);

size_t fsize(FILE *f);

int64_t dec8(int x);
int64_t decb(unknown_t p, const size_t size);

uint64_t hex8(int x);
uint64_t hex16(int x0, int x1);
uint64_t hexb(unknown_t p, const size_t size);

int strlen8(unknown_t s, const size_t maxsize);
int strlen16(unknown_t s, const size_t maxsize);
int strsize8(unknown_t s, const size_t maxsize);
int strsize16(unknown_t s, const size_t maxsize);
int strcmp8(const unknown_t s0, const char* s1, const size_t maxsize);
int strcmp16(const unknown_t s0, const char* s1, const size_t maxsize);

#endif

