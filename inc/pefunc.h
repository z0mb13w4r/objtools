#ifndef __PEFUNC_H_
#define __PEFUNC_H_

#include "buffer.h"

typedef struct pefunc_s {
  const char*   text;
  const pick_t  ords;
  const imode_t mode;
} pefunc_t, *ppefunc_t;

const char* funcpick(const ppefunc_t p, const pick_t x);
const char* funcpicknull(const ppefunc_t p, const pick_t x);

const char* get_namebyord(const pbuffer_t p, const char* name, const pick_t x);

#endif

