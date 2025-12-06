#ifndef __HASH_H_
#define __HASH_H_

#include "defines.h"

//#include <openssl/bio.h>
//#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/ripemd.h>


int md5(const unknown_t p, const size_t size, puchar_t md);
int sha1(const unknown_t p, const size_t size, puchar_t md);
int sha256(const unknown_t p, const size_t size, puchar_t md);
int sha512(const unknown_t p, const size_t size, puchar_t md);
int ripemd160(const unknown_t p, const size_t size, puchar_t md);

int entropy_calculate(const unknown_t p, const size_t size, double *entropy, double *min, double *max);

handle_t ssdeep_encode(unknown_t src, size_t srcsize);

#endif

