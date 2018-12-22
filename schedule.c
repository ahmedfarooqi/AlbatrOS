#include "schedule.h"
// #define DICK 69
void schedule(uint16_t terminal_num, int32_t total_terminals){
cli();
        
        memcpy((uint8_t*)(0xB8000), multi[visible_terminal], 25*80*2);
       if(global_terminal_array[0].top_pcb==NULL){//if 1 not null
                return;
        }
        // if(global_terminal_array[terminal_num].top_pcb==NULL){//if 1 not null
        //         //case where we have an empty terminal
        //         terminal_num++;
        //         terminal_num%=3;
        if(terminal_num==active_terminal){
                return;//case we we loop around
        }
        // pcbwrap_t global_terminal_array[terminal_num] = global_terminal_array[terminal_num];
        // pcbwrap_t global_terminal_array[active_terminal] = global_terminal_array[active_terminal];
        asm volatile(               
                "MOVL %%esp, %0;"         
                "MOVL %%ebp, %1;"  
                :"=r"(global_terminal_array[active_terminal].curr_esp0), "=r"(global_terminal_array[active_terminal].curr_ss0)               
                );
        if(terminal_num == 1 && global_terminal_array[1].top_pcb==NULL && t2_launch != 1){
                return;
        }
        else if(t2_launch == 1 && terminal_num == 1 && global_terminal_array[1].top_pcb==NULL){
                t2_launch++;
                active_terminal=1;
                locker=1;
                syscall_execute((uint8_t *)"shell");
                locker=0;
        }
        if(terminal_num == 2 && global_terminal_array[2].top_pcb==NULL && t3_launch != 1){
                return;
        }
        else if(t3_launch == 1 && terminal_num == 2 && global_terminal_array[2].top_pcb==NULL){
                t3_launch++;
                active_terminal=2;
                locker=1;
                syscall_execute((uint8_t *)"shell");
                locker=0;

        }

        // active_terminal=0;
        add_page_directory(_128MEGABYTE, _8MEGABYTE + (global_terminal_array[terminal_num].top_pcb->process_id * _4MEGABYTE));
        add_vid_directory(_128MEGABYTE+_4MEGABYTE, (uint32_t)multi[terminal_num]);
        // if(locker == 1){// this is a  placeholder to prevent lock from opening, 
        //         return; // and triple faulting the code
        // }
    // pcbwrap_t cur_terminal = global_terminal_array[visible_terminal];

    // if(global_terminal_array[terminal_num].top_pcb == NULL){
    //     global_terminal_array[terminal_num].top_pcb = 1;
    //     cli();ƒ
    //     printf("call\n");
    //     syscall_execute((uint8_t *) "shell");
    //     sti();
    // }
        // int32_t old_terminal;

        // if(total_terminals == 1){
        //         return;
        // }
                

        // if(terminal_num == 0 ){
        //         if(total_terminals == 2){
        //                 old_terminal = 1;
        //         }
        //         else if(total_terminals == 3){
        //                 old_terminal = 2;
        //         }
        // }
        // else if(terminal_num == 1){
        //         old_terminal = 0;
        // }
        // else if(terminal_num == 2){
        //         if(total_terminals == 2){
        //                 old_terminal = 0;
        //         }
        //         else {
        //                 old_terminal = 1;
        //         }
        // }
// cli();
        // if(global_terminal_array[terminal_num].top_pcb==NULL){//if 1 not null
        //         return;
        // }
        // // if(global_terminal_array[terminal_num].top_pcb==NULL){//if 1 not null
        // //         //case where we have an empty terminal
        // //         terminal_num++;
        // //         terminal_num%=3;
        // if(terminal_num==active_terminal){
        //         return;//case we we loop around
        // }
        // }if(global_terminal_array[terminal_num].top_pcb==NULL){//
        //         //case where we have an empty terminal
        //         terminal_num++;
        //         terminal_num%=3;
        //         if(terminal_num==active_terminal){
        //                 return;//case we we loop around
        //         }
        // }
        // if(terminal_num==active_terminal){
        //         return;//case we we loop around
        // }
        


        // asm volatile(               
        //         "MOVL %%esp, %0;"         
        //         "MOVL %%ebp, %1;"  
        //         :"=r"(cur_terminal.curr_esp0), "=r"(cur_terminal.curr_ss0)               
        //         );

        
        active_terminal = terminal_num; 
        
        tss.esp0 = (uint32_t) (_8MEGABYTE - (_8KILOBYTE * (global_terminal_array[terminal_num].top_pcb->process_id))-BUF_4);
        // new_terminal.curr_esp0;
        tss.ss0 = KERNEL_DS;
        //     cur_terminal = global_terminal_array[terminal_num];

        asm volatile(
                "MOVL %0, %%esp;"
                "MOVL %1, %%ebp;"
                :
                :"r"(global_terminal_array[terminal_num].curr_esp0), "r"(global_terminal_array[terminal_num].curr_ss0)            
                );

        // sti();
// sti();

        return;
}
