#ifndef __ARCODE_H_
#define __ARCODE_H_

#include <ar.h>

#include "buffer.h"

bool_t isAR(const pbuffer_t p);

unknown_t   ecget_ahdr(const pbuffer_t p);

#endif

