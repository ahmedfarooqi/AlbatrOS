#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#include "types.h"
#include "multiboot.h"
#include "lib.h"
#include "x86_desc.h"
#include "execute.h"

#define FILE_NAME_SIZE 32
#define RESERVED1 24
#define RESERVED2 52
#define NUM_DIR_ENTRIES 63
#define ONEKB 1023
// #define BOOT_BLOCK_ADDRESS  //YET TO BE FIGURED OUT

typedef struct dentry{
  uint8_t file_name[FILE_NAME_SIZE];
  uint32_t file_type;
  uint32_t inode_num;
  uint8_t reserved[RESERVED1];
} __attribute__((packed))dentry_t;

typedef struct boot_block{
  uint32_t dir_count;
  uint32_t inode_count; //N as per image in Appendix A
  uint32_t data_count; //D as per image in Appendix A
  uint8_t reserved[RESERVED2];
  dentry_t direntries[NUM_DIR_ENTRIES];
}  __attribute__((packed))boot_block_t;

typedef struct inode{
  int32_t length;
  int32_t data_block_num[ONEKB];
}  __attribute__((packed))inode_t;

void file_system_init(unsigned address, unsigned end_addr);
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int32_t file_open(const uint8_t* filename);
int32_t file_read(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_close(int32_t fd);
extern int32_t get_last_pid();
int32_t dir_open(const uint8_t* filename);
int32_t dir_read(int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_close(int32_t fd);

boot_block_t * boot_block; //
uint32_t start_addr;


#endif
