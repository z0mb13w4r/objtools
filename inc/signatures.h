#ifndef __SIGNATURES_H_
#define __SIGNATURES_H_

#include "defines.h"

#define MODE_SIGNATURE          (MODE_PUT0('S') | MODE_PUT1('I') | MODE_PUT2('G') | | MODE_PUT3('N'))

typedef struct signatures_s {
  smode_t   mode;

  size_t    size;
  unknown_t data;

} signatures_t, *psignatures_t;

#endif

