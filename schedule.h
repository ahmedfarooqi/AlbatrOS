#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#include "types.h"
#include "pit.h"
#include "execute.h"
#define _128MEGABYTE    0x8000000
#define _8MEGABYTE      0x800000
#define _4MEGABYTE      0x400000
#define _8KILOBYTE      0x2000
#define MAXLENGTH       32
#define BUF_4            4
int32_t locker, lock_main;

void schedule(uint16_t terminal_num, int32_t total_terminals);

#endif
