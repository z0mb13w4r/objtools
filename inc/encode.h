#ifndef __ENCODE_H_
#define __ENCODE_H_

#include "defines.h"

handle_t bin8_encode(unknown_t src, size_t srcsize);
handle_t bin16_encode(unknown_t src, size_t srcsize);
handle_t bin32_encode(unknown_t src, size_t srcsize);

handle_t dec8_encode(unknown_t src, size_t srcsize);
handle_t dec16_encode(unknown_t src, size_t srcsize);
handle_t dec32_encode(unknown_t src, size_t srcsize);

handle_t hex8_encode(unknown_t src, size_t srcsize);
handle_t hex16_encode(unknown_t src, size_t srcsize);
handle_t hex32_encode(unknown_t src, size_t srcsize);

handle_t base32_encode(unknown_t src, size_t srcsize);
handle_t base58_encode(unknown_t src, size_t srcsize);
handle_t base64_encode(unknown_t src, size_t srcsize);

#endif

