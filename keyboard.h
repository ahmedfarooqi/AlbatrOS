#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "pit.h"

void keyboard_init();
void keyboard_handler();
void keyboard_handler2(int input_key);
void keyboard_handler_assembly(); 
void backspace();
#define KEYBOARDBUFSIZE 128
extern volatile uint8_t keyboard_buffer_index; //use a struct for multi-terminal setup?
extern volatile uint8_t keyboard_buffer[KEYBOARDBUFSIZE]; //global buffer for keyboard
extern void reset_pos();
extern void clear_buffer();
extern int32_t terminal_read(int32_t fd, const void* buf, int32_t nbytes);
extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

void update_term_key_buf(const void * buf, const uint32_t buf_index);
// pcbwrap_t global_terminal_array[NUM_TERMINALS];

int enter_flag;
#endif /* KEYBOARD_H */

