#ifndef TESTS_H
#define TESTS_H
#include "rtc.h"
// test launcher
void launch_tests();
extern int32_t rtc_read(int32_t fd, const void* buf, int32_t nbytes); 
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);
extern void add_page_directory(uint32_t virt_addr, uint32_t phys_addr);
#endif /* TESTS_H */
