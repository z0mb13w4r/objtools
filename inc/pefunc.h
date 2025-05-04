#ifndef __PEFUNC_H_
#define __PEFUNC_H_

#include "buffer.h"

#define PEFUNC_UNKNOWN                 (0)
#define PEFUNC_INFO                    (1)
#define PEFUNC_ALERT                   (2)
#define PEFUNC_CRITICAL                (3)
#define PEFUNC_BLACKLIST               (4)

typedef struct pefunc_s {
  const char*   text;
  const pick_t  ords;
  const imode_t mode;
  const char*   desc;
} pefunc_t, *ppefunc_t;

ppefunc_t funcchoice(const ppefunc_t p, const char* name);

const char* funcpick(const ppefunc_t p, const pick_t x);
const char* funcpicknull(const ppefunc_t p, const pick_t x);

const char* get_ORDDLL(const pbuffer_t p, const char* name, const pick_t x);

imode_t get_MODEDLL(const pbuffer_t p, const char* name);

#endif

