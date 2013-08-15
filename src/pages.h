#ifndef __PAGES_H__
#define __PAGES_H__

#define PAGE_DIRENT_SIZE 4096
#define PAGE_DIRENT_COUNT (PAGE_DIRENT_SIZE / sizeof(uintptr_t))

// This is currently a 2MB mask used for computing relative offsets into a pagedir given an absolute address
#define PAGE_DIRENT_MASK (PAGE_DIRENT_SIZE*PAGE_DIRENT_COUNT - 1)

#define PAGE_SIZE 0x1000

// This offset is used for differentiating between the actual page table addresses and lower modifier bits
#define PAGE_MASK ~((uint64_t)(PAGE_SIZE - 1))

typedef enum {
  PAGE_PRESENT = 1 << 0,
  PAGE_WRITEABLE = 1 << 1,
  PAGE_USER = 1 << 2,
  PAGE_WTC = 1 << 3,
  PAGE_CACHE_DISABLE = 1 << 4,
  PAGE_ACCESSED = 1 << 5,
  PAGE_SIZEBIT = 1 << 6,
} page_flag_e;

uintptr_t *get_pagedir(uintptr_t *pml4, int ipdpt, int ipdt, int ipt);

#endif
