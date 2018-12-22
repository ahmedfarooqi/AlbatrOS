#ifndef ASM
#ifndef _IDT_H
#define _IDT_H

#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"
#include "execute.h"
#include "pit.h"

extern void keyboard_handler();
extern void rtc_handler();
extern void execute_handler();
extern void pit_handler();
// extern uint32_t syscall_execute(const uint8_t * command);
// extern keyboard_handler_assembly;
void init_idt();
void div_zero();
void reserved();
void nmi();
void breakpoint();
void overflow();
void bounds_range();
void invalid_opcode();
void device_not_available();
void double_fault();
void coprocessor_segment();
void invalid_TSS();
void segment_not_present();
void stack_seg_fault();
void general_protection_fault();
void page_fault();
void math_fault();
void alignment_check();
void machine_check();
void floating_pt_exception();
void user_defineable();
void filling();

#endif
#endif
