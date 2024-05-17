#include "paging.h"




/*
 * init_paging()
 *   DESCRIPTION: Initializes paging setup
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes paging
 */

void init_paging() {
    unsigned int i = 0;
    for (i = 0; i < PAGE_STORAGE; i++) {
        pagedir[i] = 146; //rw, pcd, pagesize. 
        pagetab[i] = 2; 
    }
    pagedir[0] = (((unsigned int)pagetab)) | 7; //shift by 12, present, r/w, user
    pagedir[1] = ((unsigned int) KERNELMEMLOC) | 131; //shift by 22, present, r/w, pagesize
    pagetab[VIRTUALMEM >> 12] = ((unsigned int)VIDTOPHYS) | 3; //present, r/w
    /*for (i = 0; i < MAX_TERM; i++) {
        pagetab[(VIDTOPHYS + PAGE_ALIGN * (i + 1)) >> 12] =  (VIDTOPHYS) | 3;
    }*/

    pagetab[(TERMINAL1) >> 12] = ((unsigned int)TERMINAL1) | 3;
    pagetab[(TERMINAL2) >> 12] = ((unsigned int)TERMINAL2) | 3;
    pagetab[(TERMINAL3) >> 12] = ((unsigned int)TERMINAL3) | 3;

    asm_pagedir((uint32_t) pagedir);

    
    
}

/*
 * asm_pagedir()
 *   DESCRIPTION: Initializes paging setup
 *   INPUTS: a - page directory
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sets assembly registers according to address of a, initializes paging
 */
inline void asm_pagedir(uint32_t a)
{
    asm volatile(
        "movl %0, %%eax;"
        "movl %%eax, %%cr3;"
        "movl %%cr4, %%eax;"
        "orl $0x10, %%eax;"
        "movl %%eax, %%cr4;"
        "movl %%cr0, %%eax;"
        "orl $0x80000000, %%eax;"
        "movl %%eax, %%cr0;"
        :
        : "r"(a)
        : "eax");
}

/*
 * flush_tlb()
 *  DESCRIPTION: Flushes the Translation Lookaside Buffer
 *  INPUTS: None
 *  OUTPUTS: None
 *  RETURN VALUE: None
 *  SIDE EFFECTS: PDEs/PTEs without G (global) bit set will be flushed.
*/
inline void flush_tlb()
{
    asm volatile(
        "movl %%cr3,%%eax;"
        "movl %%eax,%%cr3;"
        :
        :
        :"eax", "cc"
    );
}

