#!/bin/bash

PRGNAMENG=./convert-ng

echo Kym~humr | ${PRGNAMENG} xor8{4} sub8{8} -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} base64d -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} hex32e -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} hex16e -
echo R2VvcmdlIG9mIHRoZSBKdW5nbGU= | ${PRGNAMENG} hex8e -
echo 102, 109, 99, 100, 127, 100, 53, 62, 105, 57, 61, 106, 62, 62, 55, 110, 113, 114, 118, 39, 36, 118, 47, 35, 32, 125, 34, 46, 46, 124, 43, 124, 25, 71, 26, 71, 21, 88 | ${PRGNAMENG} dec8d inc{1} xor8{0} -
echo 0x66, 0x6d, 0x63, 0x64, 0x7f, 0x64, 0x35, 0x35, 0x69, 0x39, 0x3d, 0x6a, 0x3e, 0x3e, 0x37, 0x6e, 0x71, 0x72, 0x76, 0x27, 0x24, 0x76, 0x2f, 0x23, 0x20, 0x7d, 0x22, 0x2e, 0x2e, 0x7c, 0x2b, 0x7c, 0x19, 0x47, 0x1a, 0x47, 0x15, 0x58 | ${PRGNAMENG} hex8d inc{1} xor8{0} -

# c4n y0u c4p7u23 7h3 f149?
# -> LEET
# can you capture the flag?

# 01101100 01100101 01110100 01110011 00100000 01110100 01110010 01111001 00100000 01110011 01101111 01101101 01100101 00100000 01100010 01101001 01101110 01100001 01110010 01111001 00100000 01101111 01110101 01110100 00100001
# -> BINARY
# lets try some binary out!

# MJQXGZJTGIQGS4ZAON2XAZLSEBRW63LNN5XCA2LOEBBVIRRHOM======
# -> BASE32
# base32 is super common in CTF's
echo "base32 is super common in CTF's" | ${PRGNAMENG} base32e -
echo MJQXGZJTGIQGS4ZAON2XAZLSEBRW63LNN5XCA2LOEBBVIRRHOM====== | ${PRGNAMENG} base32d -

# RWFjaCBCYXNlNjQgZGlnaXQgcmVwcmVzZW50cyBleGFjdGx5IDYgYml0cyBvZiBkYXRhLg==
# -> BASE64
# Each Base64 digit represents exactly 6 bits of data.
echo RWFjaCBCYXNlNjQgZGlnaXQgcmVwcmVzZW50cyBleGFjdGx5IDYgYml0cyBvZiBkYXRhLg== | ${PRGNAMENG} base64d -

# 68 65 78 61 64 65 63 69 6d 61 6c 20 6f 72 20 62 61 73 65 31 36 3f
# -> HEX
# hexadecimal or base16?
echo 68 65 78 61 64 65 63 69 6d 61 6c 20 6f 72 20 62 61 73 65 31 36 3f | ${PRGNAMENG} hex8d -

# Ebgngr zr 13 cynprf!
# -> ROT13
# Rotate me 13 places!
echo "Ebgngr zr 13 cynprf!" | ${PRGNAMENG} rot13 -

# *@F DA:? >6 C:89E C@F?5 323J C:89E C@F?5 Wcf E:>6DX
# -> ROT47
# You spin me right round baby right round (47 times)
echo "*@F DA:? >6 C:89E C@F?5 323J C:89E C@F?5 Wcf E:>6DX" | ${PRGNAMENG} rot47 -

# 85 110 112 97 99 107 32 116 104 105 115 32 66 67 68
# -> DECIMAL
# Unpack this BCD
echo 85 110 112 97 99 107 32 116 104 105 115 32 66 67 68 | ${PRGNAMENG} dec8d -

# MVQXG6K7MJQXGZJTGI======
# -> BASE32
# easy_base32
echo "easy_base32" | ${PRGNAMENG} base32e -
echo MVQXG6K7MJQXGZJTGI====== | ${PRGNAMENG} base32d -

# NjZMKVhATl1EcEI2Jio4Q0xuVy1EZSo5ZkFLV0M6QVUtPFpGQ0InIkReYg==
# -> BASE64 -> BASE85 -> VIGENERE DECODE (tango)
# I luv vigenere cipher (eqf: tnhsv)

# TVJYWEtZVE1NVlBXRVlMVE1WWlE9PT09
# -> BASE64 -> BASE32
# double_bases

# GM4HOU3VHBAW6OKNJJFW6SS2IZ3VAMTYORFDMUC2G44EQULIJI3WIVRUMNCWI6KGK5XEKZDTN5YU2RT2MR3E45KKI5TXSOJTKZJTC4KRKFDWKZTZOF3TORJTGZTXGNKCOE
# -> BASE32 -> BASE58 -> HEX -> BASE64
# base16_is_hex

# PJXHQ4S7GEZV6ZTDOZQQ====
# -> BASE32 -> ROT13
# make_13_spin

# 2W1V8GQ5TELfYurYCZvEQV1q6RhagcEwZgi7n25vVvS
# -> BASE58
# base58 is super common in CTF's
echo "2W1V8GQ5TELfYurYCZvEQV1q6RhagcEwZgi7n25vVvS" | ${PRGNAMENG} base58d -

#echo 'egmpeg"md"vjg"hwleng' | ${PRGNAMENG} xor8brute -

