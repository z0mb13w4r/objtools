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

typedef struct peobject_s {
  const char* name;
  ppefunc_t   func;
} peobject_t, *ppeobject_t;

ppefunc_t peget_FUNC(const ppefunc_t p, const char* name);

const char* peget_ORDDLL(const pbuffer_t p, const char* name, const pick_t x);

imode_t peget_MODEFUNC(const pbuffer_t p, const char* name);

#endif

