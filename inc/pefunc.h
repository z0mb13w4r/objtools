#ifndef __PEFUNC_H_
#define __PEFUNC_H_

#include "buffer.h"

#define PEFUNC_NONE                    U64MASK_NONE
#define PEFUNC_BLACKLIST               U64MASK(0)
#define PEFUNC_CRITICAL                U64MASK(1)
#define PEFUNC_ALERT                   U64MASK(2)
#define PEFUNC_INFO                    U64MASK(3)

typedef struct pefunc_s {
  const char*   text;
  const pick_t  ords;
  const imode_t mode;
  const char*   desc;
} pefunc_t, *ppefunc_t;

const char* funcpick(const ppefunc_t p, const pick_t x);
const char* funcpicknull(const ppefunc_t p, const pick_t x);

const char* peget_namebyord(const pbuffer_t p, const char* name, const pick_t x);

#endif

