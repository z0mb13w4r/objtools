#ifndef __CRC_H_
#define __CRC_H_

#include "defines.h"

#define CRC_DEF8       (0xff)
#define CRC_DEF16      (0xffff)
#define CRC_DEF32      (0xffffffff)
#define CRC_ITU_V41    (0x6363)
#define CRC_ISO_3309   (0xffff)

uchar_t  crc8_update(uchar_t crc8, uchar_t next);
uchar_t  crc8_calculate(uchar_t seed8, puchar_t data, size_t datasize);

ushort_t crc16_update(ushort_t crc16, uchar_t next);
ushort_t crc16_calculate(ushort_t seed16, puchar_t data, size_t datasize);

ulong_t  crc32_update(ulong_t crc32, ulong_t next);
ulong_t  crc32_calculate(ulong_t seed32, puchar_t data, size_t datasize);

ushort_t csc_update_crc(uchar_t next, ushort_t crc);
ushort_t csc_compute_crc(ushort_t seed16, puchar_t data, size_t datasize);
bool_t   csc_check_crc(ushort_t seed16, puchar_t data, size_t datasize);
void     csc_compute_append_crc(ushort_t seed16, puchar_t data, size_t datasize);

#endif

