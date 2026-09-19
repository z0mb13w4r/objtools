#!/usr/bin/env python3
import sys
import binascii
import struct

UBI_HEADER = b'UBI#'
x0 = 0

if len(sys.argv) < 1:
    print("Usage: ubi-magic.py <bin>")
    sys.exit(1)

raw_file_path = sys.argv[1]
out_of_ubi = True

try:
  with open(raw_file_path, 'rb') as raw_file:
    rawbin = raw_file.read()
    for x in range(0, len(rawbin), 0x20000):
    #for x in range(0, len(rawbin)):
      magic = rawbin[x:x + 4]
      if magic == UBI_HEADER:
        if not out_of_ubi:
          print(' 0x%08x 0x%08x' % (x - 1, x - x0))

        print('%s 0x%08x' % (rawbin[x:x + 4].decode('utf-8'), x), end='')
        out_of_ubi = False
        x0 = x

  if not out_of_ubi:
    print(' 0x%08x 0x%08x' % (x - 1, x - x0))

  raw_file.close()

except Exception as e:
  print(e)