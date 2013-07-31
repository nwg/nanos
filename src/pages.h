#ifndef __PAGES_H__
#define __PAGES_H__

#define PAGE_DIRENT_SIZE 4096
#define PAGES_BASE_SIZE (4096*3)
#define PAGE_SIZE 0x1000

typedef enum {
  PAGE_PRESENT = 1 << 0,
  PAGE_WRITEABLE = 1 << 1,
  PAGE_USER = 1 << 2,
  PAGE_WTC = 1 << 3,
  PAGE_CACHE_DISABLE = 1 << 4,
  PAGE_ACCESSED = 1 << 5,
  PAGE_SIZEBIT = 1 << 6,
} page_flag_e;

#endif
