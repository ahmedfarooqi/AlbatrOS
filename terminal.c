/* terminal.c - the C part of the terminal
 * vim:ts=4 noexpandtab
 */

#include "terminal.h"
// #include "syscalls/ece391.c"

/* Terminal Init
 * Description: initializes the terminal
 * Inputs/Outputs: --
 * Effects: terminal ready to be written and read from
 */
void terminal_init(){
    clear_buffer();
    return;
}

/* For terminal, nothing happens for terminal_close
The call should find the directory entry corresponding to the
named file, allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,
RTC device, or regular file). If the named file does not exist or no descriptors are free, the call returns -1.
*/
int32_t terminal_open(const uint8_t* filename){
    return 0;
}

/* For terminal, nothing happens for terminal_open
The close system call closes the specified file descriptor and makes it available for return from later calls to open.
You should not allow the user to close the default descriptors (0 for input and 1 for output). Trying to close an invalid
descriptor should result in a return value of -1; successful closes should return 0.
*/
int32_t terminal_close(int32_t fd){
    return 0;
}


/*  Function: terminal_read
 *  Description: Function adds data from keyboard buffer to terminal buffer after
 *               enter is pressed
 *  Inputs: Pointer to the buffer and size of buffer
 *  Outputs: number of bytes read
 *  Side Effects: clear key buffer
 */
int32_t terminal_read(int32_t fd, const void * buf, int32_t nbytes){
    int i = 0;
    uint32_t length = 0;
    // if(*buf == NULL){return 0;}
    while(global_terminal_array[visible_terminal].enter_flag == 0){}
    global_terminal_array[visible_terminal].enter_flag = 0; //required
    // length = nbytes;
    uint8_t * terminal_buffer = (uint8_t *)buf; //should this be typecasted to char* for program compatibility?

    for(i = 0; i < global_terminal_array[visible_terminal].cur_keyboard_index; i++){
        terminal_buffer[i] = global_terminal_array[visible_terminal].cur_keyboard_buf[i];//copy keyboard buffer into a temp buffer
        // if(getX()==79){putc('\n');} 
        // if(terminal_buffer[i]!=NULL){
        //    putc((char)terminal_buffer[i]);
        // }
        length++;
    }
    
    clear_buffer(); //clear the keyboard buffer 
    
    // putc(terminal_buffer[0]);    
    return length; //return bytes copied
    
}


/*
// In the case of the terminal, all data should be displayed to the screen immediately
// Writes to regular files should always return -1 to indicate failure since the file system is read-only.
// The call returns the number of bytes written, or -1 on failure.*/
/*  Funtion: terminal_write
 *  Description: print buffer to screen
 *  Inputs: pointer to buffer and nbytes to write 
 *  Outputs: number of bytes displayed
 *  Effects: writes to screen
 */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){ //should handle multi terminal later?
    uint8_t * buffy;
    buffy = (uint8_t*)buf;
    int32_t i = 0;
    int32_t read_bytes = 0;
    // int32_t null_ctr = 0;
    // cli();
    for (i = 0; i < nbytes; i++){
        // if(i == nbytes /3){b8_map();}
        // if(i == 2* nbytes / 3){b8_map();}
        // if (buffy[i] == NULL){
        //     null_ctr++;
        // }
        // else{
        //     putc(buffy[i]);
        // }
        putc(buffy[i]);
        read_bytes++;

    }
    // sti();
    return read_bytes;
}

/* clear_buffer()
 *  Description: clears the keyboard buffer and sets index to 0
 *  Inputs: none
 *  Outputs: none
 *  Side Effects: buffer is cleared
 */
void clear_buffer(){
    int i;
    for(i = 0; i < KEYBUFSIZE; i++){
        global_terminal_array[visible_terminal].cur_keyboard_buf[i] = NULL;
    }
    global_terminal_array[visible_terminal].cur_keyboard_index = 0;
}

/*  reset_pos()
 *  Function: resets the position of the screen to 0,0, useful after clearing screen with ctrl+L
 *  Inputs/Outputs: --
 */
void reset_pos(){//holds screen x and y for the terminal? might be useful when we have multiple terminals
    screen_x_terminal = 0;
    screen_y_terminal = 0;
}

