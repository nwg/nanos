#!/usr/bin/env python

import struct
import sys

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
      + gdt(0, 0xFFFFF, 0b10010010, 0b1100)

  sys.stdout.write(gdt_data)
