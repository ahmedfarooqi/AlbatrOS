#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "lib.h"
#include "keyboard.h"
#define KEYBUFSIZE 128

    // typedef struct terminal_struct{
        
    // } terminal_struct; //make a struct for multi terminals?

void terminal_init();
int32_t terminal_open(const uint8_t* filename);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, const void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

void clear_buffer();
void reset_pos();
int32_t add_to_buffer();

extern int enter_flag;
uint32_t screen_x_terminal;
uint32_t screen_y_terminal;
volatile uint8_t keyboard_buffer_index; //use a struct for multi-terminal setup?
volatile uint8_t keyboard_buffer[KEYBUFSIZE]; //global buffer for keyboard
unsigned char terminal_buffer[KEYBUFSIZE];

#endif
