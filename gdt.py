#!/usr/bin/env python

import struct
import sys
from itertools import izip_longest

def gdt(base, limit, access, flags):
  '''
    base: 32-bit base address
    limit: 20-bit limit, in bytes or pages depending on granularity bit in flags
    access (8 bits), see http://wiki.osdev.org/Global_Descriptor_Table
    flags (4 bits), see http://wiki.osdev.org/Global_Descriptor_Table
  '''

  base_low = base & 0xFFFF
  base_mid = (base >> 16) & 0xFF
  base_high = (base >> 24) & 0xFF

  limit_low = limit & 0xFFFF
  limit_high = (limit >> 16) & 0xF

  limit_and_flags = limit_high | (flags << 4)

  return struct.pack('<HHBBBB', limit_low, base_low, base_mid, access, limit_and_flags, base_high)

if __name__ == '__main__':
  # nil, code, data
  gdt_data = \
      gdt(0, 0, 0, 0) \
      + gdt(0, 0xFFFFF, 0b10011010, 0b1100) \
      + gdt(0, 0xFFFFF, 0b10010010, 0b1100) \
      + gdt(0, 0xFFFFF, 0b11111010, 0b1100) \
      + gdt(0, 0xFFFFF, 0b11110010, 0b1100) \
      + gdt(0xaabbccdd, 0xeeeef, 0b11101001, 0b0000) \
      + gdt(0xaabbccdd, 0x11223, 0b11111111, 0b1111) \

  def grouper(n, iterable, fillvalue=None):
    "Collect data into fixed-length chunks or blocks"
    # grouper(3, 'ABCDEFG', 'x') --> ABC DEF Gxx
    args = [iter(iterable)] * n
    return izip_longest(fillvalue=fillvalue, *args)

  for group in grouper(8, gdt_data):
    bytes = [ '0x%02x' % ord(c) for c in group ]
    print ', '.join(bytes)
