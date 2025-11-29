#ifndef __DECODE_H_
#define __DECODE_H_

#include "defines.h"

#define AES_MODE_ECB128      (0)
#define AES_MODE_ECB192      (1)
#define AES_MODE_ECB256      (2)

#define AES_MODE_CBC128      (3)
#define AES_MODE_CBC192      (4)
#define AES_MODE_CBC256      (5)

#define AES_MODE_CFB128      (6)
#define AES_MODE_CFB192      (7)
#define AES_MODE_CFB256      (8)

#define AES_MODE_XTS128      (9)
#define AES_MODE_XTS256      (10)

#define AES_MODE_OFB128      (11)
#define AES_MODE_OFB192      (12)
#define AES_MODE_OFB256      (13)

#define AES_MODE_CTR128      (14)
#define AES_MODE_CTR192      (15)
#define AES_MODE_CTR256      (16)

int not8(const unknown_t p, const size_t size);
int not16(const unknown_t p, const size_t size);
int not32(const unknown_t p, const size_t size);

int rot5(const unknown_t p, const size_t size);
int rot13(const unknown_t p, const size_t size);
int rot18(const unknown_t p, const size_t size);
int rot47(const unknown_t p, const size_t size);

int add8(const unknown_t p, const uint8_t v, const size_t size);
int rol8(const unknown_t p, const uint8_t v, const size_t size, const int32_t step);
int ror8(const unknown_t p, const uint8_t v, const size_t size, const int32_t step);
int shl8(const unknown_t p, const uint8_t v, const size_t size, const int32_t step);
int shr8(const unknown_t p, const uint8_t v, const size_t size, const int32_t step);
int sub8(const unknown_t p, const uint8_t v, const size_t size);
int xor8(const unknown_t p, const uint8_t v, const size_t size, const int32_t step);

int add16(const unknown_t p, const uint16_t v, const size_t size);
int rol16(const unknown_t p, const uint16_t v, const size_t size, const int32_t step);
int ror16(const unknown_t p, const uint16_t v, const size_t size, const int32_t step);
int shl16(const unknown_t p, const uint16_t v, const size_t size, const int32_t step);
int shr16(const unknown_t p, const uint16_t v, const size_t size, const int32_t step);
int sub16(const unknown_t p, const uint16_t v, const size_t size);
int xor16(const unknown_t p, const uint16_t v, const size_t size, const int32_t step);

int add32(const unknown_t p, const uint32_t v, const size_t size);
int rol32(const unknown_t p, const uint32_t v, const size_t size, const int32_t step);
int ror32(const unknown_t p, const uint32_t v, const size_t size, const int32_t step);
int shl32(const unknown_t p, const uint32_t v, const size_t size, const int32_t step);
int shr32(const unknown_t p, const uint32_t v, const size_t size, const int32_t step);
int sub32(const unknown_t p, const uint32_t v, const size_t size);
int xor32(const unknown_t p, const uint32_t v, const size_t size, const int32_t step);

int aes_encrypt(const int mode, puchar_t src, const size_t srcsize,
                puchar_t key, const size_t keysize, puchar_t ivec, puchar_t dst);

handle_t bin_decode(unknown_t src, size_t srcsize);

handle_t dec8_decode(unknown_t src, size_t srcsize);
handle_t dec16_decode(unknown_t src, size_t srcsize);
handle_t dec32_decode(unknown_t src, size_t srcsize);

handle_t hex_decode(unknown_t src, size_t srcsize);

handle_t base32_decode(unknown_t src, size_t srcsize);
handle_t base58_decode(unknown_t src, size_t srcsize);
handle_t base64_decode(unknown_t src, size_t srcsize);

handle_t vigenere_decode(unknown_t src, size_t srcsize, unknown_t key, size_t keysize);

uint64_t sleb128_decode(unknown_t src, size_t srcsize);
uint64_t uleb128_decode(unknown_t src, size_t srcsize);

#endif

