#ifndef EXECUTE_H
#define EXECUTE_H

#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "filesystem.h"
#include "paging.h"
#include "x86_desc.h"
#include "terminal.h"
#include "rtc.h"
#include "keyboard.h"
#include "pit.h"

#define ONEKILOBYTE 1024
#define FOURKILOBYTE 4096
#define FDTSIZE 8
#define ARGSIZE 128
#define NUMBER_TERMINALS 3

typedef struct operations{//structure for operations table
    int32_t (*open) (const uint8_t* filename);
    int32_t (*close) (int32_t fd);
    int32_t (*read) (int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
    // int32_t (*getargs) (uint8_t* buf, int32_t nbytes);
} operations_t;

typedef struct fdt_struct{//structure of the FDT
    operations_t op_table;
    uint32_t inode_number;
    uint32_t file_position;
    uint32_t flags;
} fdt_t;

typedef struct pcb_struct{ //structure for the PCB
    uint32_t process_id;
    int32_t parent_id;
    uint32_t parent_esp;
    uint32_t parent_ebp;
    uint8_t  arguments[ARGSIZE];//CHANGE THIS FROM 123
    fdt_t file_desc_table[FDTSIZE];
    int32_t terminal_number;
} pcb_t;


typedef struct pcbwrap_struct{
    pcb_t * top_pcb;
    uint8_t cur_keyboard_buf[ARGSIZE];
    uint32_t cur_keyboard_index;
    uint32_t cur_screen_x;
    uint32_t cur_screen_y;
    int32_t fd_curr;
    uint32_t curr_esp0;
    uint32_t curr_ss0;
    int32_t enter_flag;
} pcbwrap_t;

pcbwrap_t global_terminal_array[NUMBER_TERMINALS];
uint32_t t2_launch, t3_launch;

// pcbwrap_t globalarr[3];

// uint32_t process_id = 0;
uint32_t process_pagedir[ONEKILOBYTE] __attribute__((aligned (FOURKILOBYTE)));
uint32_t ret_neg_one();
int32_t ret_neg_one_open(const uint8_t* filename);
int32_t ret_neg_one_close(int32_t fd);
int32_t ret_neg_one_read(int32_t fd, const void* buf, int32_t nbytes);
int32_t ret_neg_one_write(int32_t fd, const void* buf, int32_t nbytes);
uint32_t CONTEXT_SWITCH(uint32_t entry_point);
uint32_t syscall_execute(const uint8_t * command);
// void switch_to_user_mode(uint32_t entry_point);
extern int enter_flag;
int32_t get_pid();
int32_t get_last_pid();
void syscall_handler_assembly();
int32_t syscall_halt (uint8_t status);
int32_t syscall_read (int32_t fd, void* buf, int32_t nbytes);
int32_t syscall_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t syscall_open (const uint8_t* filename);
int32_t syscall_close (int32_t fd);

int32_t syscall_getargs (uint8_t* buf, int32_t nbytes);
int32_t syscall_vidmap(uint8_t** screen_start);


#endif /* EXECUTE_H */
