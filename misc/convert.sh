#!/bin/bash

PRGNAMENG=./convert-ng

echo Kym~humr | ${PRGNAMENG} xor8{4} sub8{8} -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} base64d -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} hex32e -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} hex16e -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} hex8e -
echo 102, 109, 99, 100, 127, 100, 53, 62, 105, 57, 61, 106, 62, 62, 55, 110, 113, 114, 118, 39, 36, 118, 47, 35, 32, 125, 34, 46, 46, 124, 43, 124, 25, 71, 26, 71, 21, 88 | ${PRGNAMENG} dec8d inc{1} xor8{0} -
echo 0x66, 0x6d, 0x63, 0x64, 0x7f, 0x64, 0x35, 0x35, 0x69, 0x39, 0x3d, 0x6a, 0x3e, 0x3e, 0x37, 0x6e, 0x71, 0x72, 0x76, 0x27, 0x24, 0x76, 0x2f, 0x23, 0x20, 0x7d, 0x22, 0x3e, 0x3e, 0x7c, 0x2b, 0x7c, 0x19, 0x47, 0x1a, 0x47, 0x15, 0x58 | ${PRGNAMENG} hex8d inc{1} xor8{0} -
