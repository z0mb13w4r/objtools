#!/bin/bash

PRGNAMENG=./convert-ng

echo Kym~humr | ${PRGNAMENG} sub8{8} xor8{4} -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} base64d -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} hex32e -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} hex16e -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} hex8e -
echo 102, 109, 99, 100, 127, 100, 53, 62, 105, 57, 61, 106, 62, 62, 55, 110, 113, 114, 118, 39, 36, 118, 47, 35, 32, 125, 34, 46, 46, 124, 43, 124, 25, 71, 26, 71, 21, 88 | ${PRGNAMENG} dec8d 'inc(1)' 'xor(0)' -
