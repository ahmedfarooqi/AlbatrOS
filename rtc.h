#ifndef _RTC_H
#define _RTC_H

#include "lib.h"
#include "i8259.h"

void rtc_initialize();
void rtc_handler(); 
void rtc_handler_assembly(); 

int32_t rtc_open(const uint8_t* filename);
int32_t rtc_close(int32_t fd);
int32_t rtc_read(int32_t fd, const void* buf, int32_t nbytes); 
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);


#endif
