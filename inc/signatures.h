#ifndef __SIGNATURES_H_
#define __SIGNATURES_H_

#include "defines.h"

#define SIGNATURE_NONE         (U32MASK_NONE)
#define SIGNATURE_EP_ONLY      (U32MASK(0))

#define SIGNATURE_MAGIC0       (MODE_PUT0('S') | MODE_PUT1('G') | MODE_PUT2('N') | MODE_PUT3('0'))
#define SIGNATURE_FLAG         (MODE_PUT0('F') | MODE_PUT1('L') | MODE_PUT2('A') | MODE_PUT3('G'))
#define SIGNATURE_NAME         (MODE_PUT0('N') | MODE_PUT1('A') | MODE_PUT2('M') | MODE_PUT3('E'))
#define SIGNATURE_SIGNATURE    (MODE_PUT0('S') | MODE_PUT1('I') | MODE_PUT2('G') | MODE_PUT3('N'))

#define MODE_SIGNATURE          (MODE_PUT0('S') | MODE_PUT1('I') | MODE_PUT2('G') | | MODE_PUT3('N'))

typedef struct signatures_s {
  smode_t   mode;

  size_t    size;
  unknown_t data;

} signatures_t, *psignatures_t;

int signature_pecode(handle_t p, const char* data, const size_t datasize, const uint32_t mode);

#endif

