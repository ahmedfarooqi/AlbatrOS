#include "pit.h"

void pit_init(){
    pit_counter = 0;
    t2_launch = 0;
    locker = 0;
    global_terminal_array[0].top_pcb=NULL;
    global_terminal_array[1].top_pcb=NULL;
    global_terminal_array[2].top_pcb=NULL;
    t3_launch = 0;
    lock_main = 0;
    outb(INIT_VAL, PIT_PORT);
    uint16_t frequency = BASE_FREQ; //50 hertz, time_slice = 20ms
    uint32_t pit_reload_val = 0;
    pit_reload_val = MAX_FREQ/(3*frequency);
    outb(pit_reload_val & 0xff, PIT_REWRITE_PORT_CHANNEL_0);
    outb(pit_reload_val >> HI8SHIFT, PIT_REWRITE_PORT_CHANNEL_0);
    enable_irq(PIT_IRQ);
    // enable_irq(0);

    return;
}
// volatile int initialize = 0;
//values from https://wiki.osdev.org/Programmable_Interval_Timer
void pit_handler(){
    send_eoi(PIT_IRQ); //pit irq 0
    // printf("pit_called\n");
    // if(get_pid == 0){
        // syscall_execute((uint8_t *) "shell");
        // initialize = 1;
    // }
    // if(pit_counter == 0 || pit_counter == 1 || pit_counter == 2){//initialize terminals
    // }
    int x = 1;
    if(t2_launch == 2){
        x++;
    }
    if(t3_launch == 2){
        x++;
    }

    // pit_counter++;
    // pit_counter %= 3;
    // active_terminal = pit_counter;
    schedule( pit_counter=(pit_counter+1)%3 , x);
    sti();
    // pit_counter++;

    return;

}
