// /* paging.h - Defines for useful library functions
//  * vim:ts=4 noexpandtab
//  */

#ifndef PAGING_H
#define PAGING_H

#include "types.h"
#include "execute.h"
#include "pit.h"

#define ONEKILOBYTE 1024 //lecture 15 slide 7
#define FOURKILOBYTE 4096

typedef struct terminal_pages{
    //1024 entries
    uint32_t page_directory[ONEKILOBYTE] __attribute__((aligned (FOURKILOBYTE)));
    //4kB page holding 4096/4=1024 PTE
    uint32_t page_table[ONEKILOBYTE] __attribute__((aligned (FOURKILOBYTE)));
    // uint32_t pdexecute[ONEKILOBYTE] __attribute__((aligned (FOURKILOBYTE)));
    uint32_t vidmap_page[ONEKILOBYTE] __attribute__((aligned (FOURKILOBYTE)));
}terminal_pages;
uint32_t video_page[ONEKILOBYTE] __attribute__((aligned (FOURKILOBYTE)));
void add_page_directory(uint32_t virt_addr, uint32_t phys_addr);
void add_vid_directory(uint32_t virt_addr, uint32_t phys_addr);
// extern volatile uint32_t t2_launch, t3_launch;
//initialization for paging
void page_init();
void b8_map(uint32_t switcher);

void flush_tlb();
 
#endif /* PAGING_H */
