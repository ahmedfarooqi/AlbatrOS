
#include "keyboard.h"
#include "lib.h"

#define KB_PORT 0x60
#define CAP_OFFSET 32 //if we need a capital, take the lowercase key and subtract 32 from decimal value
#define LSHIFTD   0x2A
#define RSHIFTD   0x36
#define BACKSPACE 0x0E
#define CAPSLOCK  0x3A
#define ENTER	  	0x1C
#define CTRLD   	0x1D
#define ALTD     	0x38
#define LSHIFTU	  0xAA
#define RSHIFTU	  0xB6
#define CTRLU	  	0x9D
#define ALTU  		0xB8
#define LINESIZE79  79
#define F1          59
#define F2          60
#define F3          61


int ctrl_flag = 0;
int alt_flag = 0;
// unsigned char keyboard_buffer[KEYBOARDBUFSIZE]; //global buffer for keyboard
// unsigned int buffer_index = 0; //use a struct for multi-terminal setup?
int scan_table = 0;
int switcher = 1;
unsigned char key;


unsigned char scan_map[4][128] = //scan table
{
  {/*no cap no shift*/
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	// 9
  '9', '0', '-', '=', '\b',	//Backspace
  '\t',			// Tab
  'q', 'w', 'e', 'r',	// 19
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	// Enter key
    0,			// 29   - Control
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	// 39
 '\'', '`',   0,		// Left shift 42
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			// 49
  'm', ',', '.', '/',   0,				// Right shift 54
  '*',
    0,	// Alt
  ' ',	// Space bar
    0,	// Caps lock
    0,	// 59 - F1 key ... >
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	// < ... F10
    0,	// 69 - Num lock
    0,	// Scroll Lock
    0,	// Home key
    0,	// Up Arrow
    0,	// Page Up
  '-',
    0,	// Left Arrow
    0,
    0,	// Right Arrow
  '+',
    0,	// 79 - End key
    0,	// Down Arrow
    0,	// Page Down
    0,	// Insert Key
    0,	// Delete Key
    0,   0,   0,
    0,	// F11 Key
    0,	// F12 Key
    0,	// All other keys are undefined
  },
  {/*no cap shift*/
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	// 9
  '(', ')', '_', '+', '\b',	//Backspace
  '\t',			// Tab
  'Q', 'W', 'E', 'R',	// 19
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	// Enter key
    0,			// 29   - Control
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	// 39
 '|', '~',   0,		// Left shift 42
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',			// 49
  'M', '<', '>', '?',   0,				// Right shift 54
  '*',
    0,	// Alt
  ' ',	// Space bar
    0,	// Caps lock
    0,	// 59 - F1 key ... >
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	// < ... F10
    0,	// 69 - Num lock
    0,	// Scroll Lock
    0,	// Home key
    0,	// Up Arrow
    0,	// Page Up
  '-',
    0,	// Left Arrow
    0,
    0,	// Right Arrow
  '+',
    0,	// 79 - End key
    0,	// Down Arrow
    0,	// Page Down
    0,	// Insert Key
    0,	// Delete Key
    0,   0,   0,
    0,	// F11 Key
    0,	// F12 Key
    0,	// All other keys are undefined
  },
  {/*cap no shift*/
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	// 9
  '9', '0', '-', '=', '\b',	//Backspace
  '\t',			// Tab
  'Q', 'W', 'E', 'R',	// 19
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',	// Enter key
    0,			// 29   - Control
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';',	// 39
 '\'', '`',   0,		// Left shift 42
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',			// 49
  'M', ',', '.', '/',   0,				// Right shift 54
  '*',
    0,	// Alt
  ' ',	// Space bar
    0,	// Caps lock
    0,	// 59 - F1 key ... >
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	// < ... F10
    0,	// 69 - Num lock
    0,	// Scroll Lock
    0,	// Home key
    0,	// Up Arrow
    0,	// Page Up
  '-',
    0,	// Left Arrow
    0,
    0,	// Right Arrow
  '+',
    0,	// 79 - End key
    0,	// Down Arrow
    0,	// Page Down
    0,	// Insert Key
    0,	// Delete Key
    0,   0,   0,
    0,	// F11 Key
    0,	// F12 Key
    0,	// All other keys are undefined
  },
  {/*cap shift*/
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	// 9
  '9', '0', '-', '=', '\b',	//Backspace
  '\t',			// Tab
  'q', 'w', 'e', 'r',	// 19
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	// Enter key
    0,			// 29   - Control
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	// 39
 '\'', '`',   0,		// Left shift 42
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			// 49
  'm', ',', '.', '/',   0,				// Right shift 54
  '*',
    0,	// Alt
  ' ',	// Space bar
    0,	// Caps lock
    0,	// 59 - F1 key ... >
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	// < ... F10
    0,	// 69 - Num lock
    0,	// Scroll Lock
    0,	// Home key
    0,	// Up Arrow
    0,	// Page Up
  '-',
    0,	// Left Arrow
    0,
    0,	// Right Arrow
  '+',
    0,	// 79 - End key
    0,	// Down Arrow
    0,	// Page Down
    0,	// Insert Key
    0,	// Delete Key
    0,   0,   0,
    0,	// F11 Key
    0,	// F12 Key
    0,	// All other keys are undefined
  }
};


/* keyboard_init
 * Inputs: void
 * Return Value: void
 * Function: initializes the interrupt for keyboard*/
void keyboard_init() {
  cli();
  int i=0;
  global_terminal_array[0].cur_keyboard_index = 0;
  global_terminal_array[1].cur_keyboard_index = 0;
  global_terminal_array[2].cur_keyboard_index = 0;
  for(i = 0; i< KEYBOARDBUFSIZE; i++){
    global_terminal_array[0].cur_keyboard_buf[i] = 0x0;//clear keyboard buffer
    global_terminal_array[1].cur_keyboard_buf[i] = 0x0;//clear keyboard buffer
    global_terminal_array[2].cur_keyboard_buf[i] = 0x0;//clear keyboard buffer
  }
  enable_irq(1); // enable interrupt for keyboard
  sti();
  update_cursor();
}
/* keyboard_handler
 * Inputs: void
 * Return Value: void
 * Function: echos the pressed characters on the screen.
 *           Called from inline assembly in handler.s
 */
void keyboard_handler(){
    disable_irq(1);
    // cli();
    int flag = inb(KB_PORT); //fetch  scancode for keyboard pressed
    // check edge cases
    keyboard_handler2(flag);
      if(ctrl_flag == 1){ //to do TO DO TODO T O D O t o d o
        if(key == 'l' || key == 'L'){
          clear();
          reset_pos(); //shouldnt clear buffer, copy last line to top? idk TODO
          putX(0);
          putY(0);
          update_cursor(); 
          clear_buffer();
        }
      }
      if(alt_flag == 1){ //testing functionality of the terminal write
        switch(flag){
          case F1:
            // printf("t1\n");ß
            // f1();
            b8_map(0x0);
            break;
          case F2:
            // printf("t2\n");
            if(t2_launch == 0){
              t2_launch = 1;
            }
            b8_map(0x1);
            // f2();
            break;
          case F3:
            if(t3_launch == 0){
                t3_launch = 1;
            }
            b8_map(0x2);
            // printf("t3\n");
            // f3();
            
            break;
        }
        // uint8_t penbuf[35] = { /*buffer to test terminal write*/
        //   '\n', ' ', ' ', '.', '~', '.', '\n',
        //   ' ', ' ', '/', 'V', '\\', '\n',
        //   ' ', '/', '/', ' ', '\\', '\\', '\n',
        //   '/', '(', ' ', ' ', ' ', ')', '\\','\n',
        //   ' ', '^', '`', '~', '`', '^', '\n'
        // };
        // terminal_write(NULL, penbuf, sizeof(penbuf)); //test for terminal write
      }
      if(flag > 0 && flag < KEYBOARDBUFSIZE && flag != RSHIFTD && flag != LSHIFTD && flag != CAPSLOCK && flag != CTRLD && flag != ALTD && flag!= BACKSPACE && alt_flag != 1){
        if(global_terminal_array[visible_terminal].cur_keyboard_index < KEYBOARDBUFSIZE){
        // if(keyboard_buffer_index<KEYBOARDBUFSIZE){
          if(key == '\n'){
            // terminal_read(NULL, keyboard_buffer, sizeof(keyboard_buffer)); //test for terminal read
            putc('\n');
            // enter_flag = 1;
            global_terminal_array[visible_terminal].enter_flag = 1;
            // clear_buffer();
          }
          else if(global_terminal_array[visible_terminal].cur_keyboard_index < KEYBOARDBUFSIZE){//echo just char
            // keyboard_buffer[keyboard_buffer_index] = scan_map[scan_table][flag];
            global_terminal_array[visible_terminal].cur_keyboard_buf[global_terminal_array[visible_terminal].cur_keyboard_index] = scan_map[scan_table][flag];
            
            putc(global_terminal_array[visible_terminal].cur_keyboard_buf[global_terminal_array[visible_terminal].cur_keyboard_index]);

            global_terminal_array[visible_terminal].cur_keyboard_index++;
            // global_terminal_array[visible_terminal].cur_keyboard_index = keyboard_buffer_index;
          }
          // else if(global_terminal_array[visible_terminal].cur_keyboard_index < KEYBOARDBUFSIZE){//echo char and new line
          //   putc(scan_map[scan_table][flag]); // echo the mapped character on screen
          //   global_terminal_array[visible_terminal].cur_keyboard_buf[global_terminal_array[visible_terminal].cur_keyboard_index] = scan_map[scan_table][flag];
          //   global_terminal_array[visible_terminal].cur_keyboard_index++;
          //   putc('\n');
          // }
          flag = 0;
        }
      }
      else{
        flag = 0; //invalid key pressed
      }
      send_eoi(1); //send end of interrupt
      // sti();
      enable_irq(1);
      return;

}

/* keyboard_handler2
 * Inputs: input_key - key pressed
 * Return Value: void
 * Function: sets the flags for special keys and sets global variable key 
 */
void keyboard_handler2(int input_key){
  // check edge cases
  key = scan_map[scan_table][input_key]; //get char from appropriate table
  // printf("key: %d\n", input_key);
  switch(input_key){//set enter flag
    case LSHIFTD:
          if(scan_table == 2){scan_table = 3;} //special case for caps and shift
          else{scan_table = 1;}
          break;
    case LSHIFTU: //release shift
          if(scan_table == 3){scan_table=2;}
          else scan_table = 0;
          break;
    case RSHIFTD:
          if(scan_table == 2){scan_table = 3;}
          else{scan_table = 1;}
          break;
    case RSHIFTU: //case for when caps enabled
          if(scan_table == 3){scan_table=2;}
          else scan_table = 0;
          break;
    case CAPSLOCK: //special cases for if shift pressed durinig caps
          if(scan_table == 2){scan_table = 0;}
          else{scan_table = 2;}
          break;
    case CTRLD:
          ctrl_flag = 1;
          break;
    case CTRLU: //if ctrl released, reset flag
          ctrl_flag = 0;
          break;
    case BACKSPACE:
          backspace(); //call backspace funtion
          break;
    case ALTD:
          alt_flag = 1;
          break;
    case ALTU: //if key released, reset flag
          alt_flag = 0;
          break;
    // case ENTER:
    //       enter_flag = 1;
    //       break;
  }
  return;
}

/* backspace
 * Inputs: none
 * Return Value: none
 * Function: clears previous char and removes it from the buffer
 *           as well as update cursor and xy positiioin 
 */
void backspace(){
  if(global_terminal_array[visible_terminal].cur_keyboard_index > KEYBOARDBUFSIZE){ //edge case
    return;
  }
  if(global_terminal_array[visible_terminal].cur_keyboard_index == 0){ //do nothing if the buffer empty
    return;
  }
  global_terminal_array[visible_terminal].cur_keyboard_buf[global_terminal_array[visible_terminal].cur_keyboard_index-1] = NULL; //clear the buffer
  global_terminal_array[visible_terminal].cur_keyboard_index--;

  putX(getX()-1); //move x back 1
  putc(' '); //clear char
  putX(getX()-1); //reset pos again
  if(key == '\n'){
    putY(getY()-1);
  }
  update_cursor(); //update cursor to new pos
  return;
}
