#include "memory.h"
#include "pages.h"
#include "sizes.h"
#include "kmem.h"
#include "asm.h"
#include "kernel.h"
#include <stdint.h>

#define DROPFLAGS(p) ( (__typeof__(p)) ((uintptr_t)p & PAGE_MASK) )
#define WITHFLAGS(p, flags) ( (__typeof__(p)) ( ((uintptr_t)p) | flags) )
#define PPPP_IDX(addr) ( (((uintptr_t)addr) >> 39) & 0x1FF )
#define PPP_IDX(addr)  ( (((uintptr_t)addr) >> 30) & 0x1FF )
#define PP_IDX(addr)   ( (((uintptr_t)addr) >> 21) & 0x1FF )
#define P_IDX(addr)    ( (((uintptr_t)addr) >> 12) & 0x1FF )

uintptr_t ****pt_alloc() {
    uintptr_t ****pml4 = kalloc_aligned(4096, 4096);
    memset(pml4, 0, 4096);
    return pml4;
}

void pt_dealloc(uintptr_t ****pppp) {
    for (int i = 0; i < 512; i++) {
        uintptr_t ***ppp = pppp[i];
        if (!ppp) continue;

        ppp = DROPFLAGS(ppp);
        for (int j = 0; j < 512; j++) {
            uintptr_t **pp = ppp[j];
            if (!pp) continue;

            pp = DROPFLAGS(pp);
            for (int k = 0; k < 512; k++) {
                uintptr_t *p = pp[k];
                if (!p) continue;
                kfree(p);
            }
            kfree(pp);
        }
        kfree(ppp);
    }

    kfree(pppp);
}

void pt_map(uintptr_t ****pppp, uintptr_t vaddr, uintptr_t paddr, size_t nbytes,
 page_flag_e flags4, page_flag_e flags3, page_flag_e flags2, page_flag_e flags1) {
    uintptr_t vaddrp = (uintptr_t)vaddr & ~0x1FF;
    uintptr_t paddrp = (uintptr_t)paddr & ~0x1FF;

    size_t b = 0;

    // 9 bits (512 entries) per level, starting at increments of 4096 in the pagedir
    for (int i = PPPP_IDX(vaddrp); b < nbytes; i++) {

        uintptr_t ***ppp = pppp[i];
        if (!ppp) {
            ppp = kalloc_aligned(4096, 4096);
        }
        pppp[i] = WITHFLAGS(ppp, flags4);

        ppp = DROPFLAGS(ppp);
        for (int j = PPP_IDX(vaddr); b < nbytes; j++) {

            uintptr_t **pp = ppp[j];
            if (!pp) {
                pp = kalloc_aligned(4096, 4096);
            }
            ppp[j] = WITHFLAGS(pp, flags3);

            pp = DROPFLAGS(pp);
            for (int k = PP_IDX(vaddr); b < nbytes; k++) {
                uintptr_t *p = pp[k];
                if (!p) {
                    p = kalloc_aligned(4096, 4096);
                }
                pp[k] = WITHFLAGS(p, flags2);

                p = DROPFLAGS(p);
                for (int l = P_IDX(vaddr); b < nbytes; l++) {
                    p[l] = WITHFLAGS(paddrp + b, flags1);
                    b += 4096;
                }
            }
        }

    }
}

uintptr_t *get_pagedir(uintptr_t *pml4, int ipdpt, int ipdt, int ipt) {
	uintptr_t *pdpt = (uintptr_t*)(pml4[ipdpt] & PAGE_MASK);
	uintptr_t *pdt = (uintptr_t*)(pdpt[ipdt] & PAGE_MASK);
	uintptr_t *pt = (uintptr_t*)(pdt[ipt] & PAGE_MASK);

	return pt;
}
