// /* paging.c - setting up paging for C
//  * vim:ts=4 noexpandtab
//  */

#include "paging.h"
#include "x86_desc.h"
#include "lib.h"

#define VIDEODIR       0xB8 /*0xB8000/4096*/
#define VIDEOADDR   0xB8000
#define FOURMEGABYTE 0x400000
#define MASK32      0x80000001
#define MASK4MBRW   0x83
#define MASK8SEVEN    0x87
#define MASKSEVEN    0x7
#define MASKRWP     0x3
#define MASKNP      0x02
#define SHIFTVAL 22
#define _33 33
#define _128MEGABYTE    0x8000000
#define _8MEGABYTE      0x800000
#define _VIDSIZE        80*25*2
#define color1  0x1A
#define color2  0x0f
#define color3  0xf0
/* page_init;
 * Inputs: none
 * Return Value: none
 * Function: sets processor registers to intialize paging and sets
 *           up initial pages for kernel and video
 */
terminal_pages terminals_pg;
void page_init(){
    int i; //loop variable
    //clear the tables
    for(i = 0; i < ONEKILOBYTE; i++){
        terminals_pg.page_directory[i] = MASKNP; //set not present
        terminals_pg.page_table[i] = (i* 0x1000) | MASKRWP;
        video_page[i]  = MASKNP;
        terminals_pg.vidmap_page[i]  = MASKNP;
    }
    active_terminal = 0;
    // visible_terminal = 0;
    terminals_pg.page_table[VIDEODIR] = (terminals_pg.page_table[VIDEODIR] | 0x1);
    // terminals_pg.page_table[VIDEODIR+1] = 0x1001 * 
    // this is page directory to the 4kb pages located @ 4MB
    // page directory for kernel which is 4MB page
    //following will hold page for video memory, at addr/4096 index

 //terminal array s
    multi[0] = (uint8_t*)(terminals_pg.page_table[VIDEODIR+1]);
    multi[1] = (uint8_t*)(terminals_pg.page_table[VIDEODIR+2]);
    multi[2] = (uint8_t*)(terminals_pg.page_table[VIDEODIR+3]);
    
    //page for video
    video_page[0] =  VIDEOADDR | MASKRWP;//rw, present
    video_page[1] =  VIDEOADDR | MASKRWP;//rw, present used for vidmap
    video_page[2] =  VIDEOADDR | MASKRWP;//rw, present used for vidmap
    
    // terminals_pg.page_table[VIDEODIR] = VIDEOADDR | MASKRWP;

   

    terminals_pg.page_directory[0] = (unsigned int)terminals_pg.page_table | MASKRWP;
    terminals_pg.page_directory[1] = FOURMEGABYTE | MASK4MBRW;
    
    //vidmap
    terminals_pg.page_directory[_33] = (unsigned int)video_page | 7;
        // terminals_pg.page_table[VIDEODIR] = (_128MEGABYTE + _8MEGABYTE + _8MEGABYTE )| MASKRWP;
    uint32_t j,k;
    int32_t color;
    
    for(j = 0; j < 3; j++){
        if(j == 1){
        color = color2;
    }
    else if(j == 2){
        color = color3;
    }
    else{
        color = color1;
    }
        for (k = 0; k < (25 * 80); k++) {
            // *(uint8_t *)(video_mem + (i << 1)) = ' ';
            *(uint8_t *)(multi[j] + (k << 1)) = ' ';
            // *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
            *(uint8_t *)(multi[j] + (k << 1) + 1) = color;
        }
    }
    // terminals_pg.page_table[VIDEODIR+1] = terminals_pg.page_table[VIDEODIR];
  /* 3-18 ia32 flag pg 57 enable paging registers -> enable paging
   * CR3 holds page directory pointer, 
   * PG  (paging) flag: bit 31 of CR0
   * PSE (page size extensions) flag: bit 4 of CR4
   */ 
    asm volatile ("                     \n\
            movl    %0, %%eax           \n\
            movl    %%eax, %%cr3        \n\
            movl    %%cr4, %%eax        \n\
            orl     $0x00000010, %%eax  \n\
            movl    %%eax, %%cr4        \n\
            movl    %%cr0, %%eax        \n\
            orl     $0x80000001, %%eax  \n\
            movl    %%eax, %%cr0        \n\
            "/*Paging flag is bit 31 represented by 0x80000001 (MASK32)*/
            :                         /*no ouputs*/
            : "r"(terminals_pg.page_directory)     /*input is page directory*/ 
            : "eax"                   /*modified registers*/
    );

}

void b8_map(uint32_t switcher){
   
    cli();
    // printf("AT: %d, toppcbtermnum: %d, VT: %d\n", active_terminal, global_terminal_array[visible_terminal].top_pcb->terminal_number, visible_terminal);
	memcpy(multi[visible_terminal],(uint8_t*)VIDEOADDR, _VIDSIZE);
	memcpy((uint8_t*)VIDEOADDR,multi[switcher], _VIDSIZE);
    // update_cursor();
    visible_terminal = switcher;//error is here
    // update_cursor();
    sti();
    // if(t2_launch == 1){
    //     syscall_execute((uint8_t *)"shell");
    // }
    // else if(t3_launch == 1){
    //     syscall_execute((uint8_t *)"shell");
    // }
    return;
    // printf("")
    //memcpy(VIDEOADDR, _128MEGABYTE+_8MEGABYTE+_8MEGABYTE, );
}

/*add_page_directory
 * maps a physical address from a virtual address
 * inputs: virt_addr - where page is to  be mapped in virtual mem
 *         phys_addr - where the pde maps to   
 * 
 */
void add_page_directory(uint32_t virt_addr, uint32_t phys_addr){
    uint32_t pde = virt_addr >> SHIFTVAL; // get the index from the passed through virtual address
    terminals_pg.page_directory[pde] = phys_addr | MASK8SEVEN; //into 128mb  virtual data
    flush_tlb();
}

void add_vid_directory(uint32_t virt_addr, uint32_t phys_addr){
    uint32_t pde = virt_addr >> SHIFTVAL; // get the index from the passed through virtual address
    terminals_pg.page_directory[pde] = (uint32_t)terminals_pg.vidmap_page | MASKSEVEN; //into 128mb  virtual data
    terminals_pg.vidmap_page[0] = phys_addr | MASKSEVEN; 
    flush_tlb();
}

/*flush_tlb
 *takes the cr3 regiter used for the tlb and flushes it
 * happens when at a new page in pd, done in paging
 * inputs: none
 * outputs: none
 * */
void flush_tlb(){
    asm volatile ("                   \n\
                    movl %%cr3, %%eax \n\
                    movl %%eax, %%cr3 \n\
                    "
                    :
                    :
                    :"eax");
}
