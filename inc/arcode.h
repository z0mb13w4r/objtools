#ifndef __ARCODE_H_
#define __ARCODE_H_

#include <ar.h>

#include "buffer.h"

bool_t isAR(const pbuffer_t p);
bool_t isARarch(const pbuffer_t p);
bool_t isARthin(const pbuffer_t p);

unknown_t   ecget_ahdr(const pbuffer_t p, const int index);
handle_t    ecget_archive(const pbuffer_t p, const int index, const size_t blocksize);

#endif

