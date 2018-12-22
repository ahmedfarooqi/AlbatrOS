#ifndef _PIT_H
#define _PIT_H

#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "schedule.h"
#include "execute.h"

//values from https://wiki.osdev.org/Programmable_Interval_Timer
#define PIT_IRQ 0
#define PIT_PORT 0x43
#define PIT_REWRITE_PORT_CHANNEL_0 0x40
#define INIT_VAL 0x36
#define HI8SHIFT 8
#define BASE_FREQ 50
#define MAX_FREQ 3579545
#define NUM_TERMINALS 3

void pit_init();
void pit_handler();
void pit_handler_assembly();

volatile uint32_t active_terminal;
volatile uint32_t visible_terminal;
volatile uint32_t pit_counter;

#endif
