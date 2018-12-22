#include "filesystem.h"

#define FOUR_KB_BLOCK 4096
#define NUM_INODES 63
#define _128MEGABYTE    0x8000000
#define _8MEGABYTE      0x800000
#define _4MEGABYTE      0x400000
#define _8KILOBYTE      0x2000
#define MAXLENGTH       32

/* file_system_init
 * Inputs: starting address of file system, ending address of file system
 * Return Value: void
 * Function: initializes file system*/
void file_system_init(uint32_t address, uint32_t end_addr){
  start_addr = address;
  boot_block = (boot_block_t *)address;
  // printf("dentries, %d\n", boot_block->dir_count);
  // printf("inodes, %d\n", boot_block->inode_count);
  // printf("dblocks, %d\n", boot_block->data_count);

}


/* read_dentry_by_index
* Inputs: index of file, directory entry of file system
* Return Value: -1 if index of of range, 0 if successful
* Function: reads directory entry using index*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry){
  if (index < 0 || index>=NUM_INODES){ //checking if index out of range
    return -1;
  }
  dentry_t curr_dentry;
  curr_dentry = boot_block->direntries[index];
  memcpy(dentry, &boot_block->direntries[index], 64);
  return 0;
}


/* read_dentry_by_name
* Inputs: name of file and directory entry of file system
* Return Value: -1 or 0 based on read success
* Function: reads directory entry using file name*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){

  if (fname == NULL || *fname == (uint8_t)""){
    return -1;
  }

  int32_t i;
  for(i=0; i<NUM_INODES; i++){
    if(strncmp((int8_t*)fname, (int8_t*)&(boot_block->direntries[i].file_name), 32) == 0){ //Check if file_name matches
      memcpy(dentry, &boot_block->direntries[i], 64);
      return 0;
    }
  }
  return -1;
}


/* read_data
* Inputs: node, offset, buffer, length of buffer
* Return Value: -1 if unsuccessful, otherwise the no of bytes we are copying
* Function: gets data from data blocks and filles buffer*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

  if(inode < 0 || inode > NUM_INODES || buf == 0){
    return -1;
  }

  uint32_t inode_address = start_addr + (inode+1)*FOUR_KB_BLOCK; //get specific inode's address
  uint32_t data_block_start_addr = start_addr + (boot_block->inode_count+1)*FOUR_KB_BLOCK; //start_addr+64*FOUR_KB_BLOCK
  uint32_t curr_d_block_addr;
  inode_t * curr_inode = (inode_t*)inode_address; //get data for the specific inode
  int32_t possible_length; //the length that we can copy
  int32_t copy_counter = 0; //count how many bytes have been copied to buf

  if (length + offset <= curr_inode->length){
    possible_length = length;
  }
  else if(length + offset > curr_inode->length){ //if file length is lesser than length given
    possible_length = curr_inode->length - offset ; //read only the length of file
  }

  int32_t i;
  int32_t j;

  for(i = 0; i < 1023; i++){ //total possible data_blocks
    curr_d_block_addr = data_block_start_addr + FOUR_KB_BLOCK * curr_inode->data_block_num[i];

    for(j = 0; j < FOUR_KB_BLOCK; j++){ //traverse through 4kb block

      memcpy(buf+copy_counter, (uint8_t*)curr_d_block_addr + j + offset, 1);
      copy_counter++;

      if (copy_counter >= possible_length){
        return copy_counter;

      }
    }
  }

  return -1;
}

/*
* get_file_size_by_index(uint32_t index)
* Inputs: Index
* Return: Size of file in bytes
* Function: Returns size of a file given the inode number
*/
int32_t get_file_size_by_index(uint32_t index){
  if (index < 0 || index>=NUM_INODES){ //checking if index out of range
    return -1;
  }
  uint32_t inode_address = start_addr + (index+1)*FOUR_KB_BLOCK; //get specific inode's address
  inode_t * curr_inode = (inode_t*)inode_address; //get data for the specific inode

  return curr_inode->length;
}


/* file_open(const uint8_t* filename)
* Inputs: void
* Return Value: 0
* Function: opens file*/
int32_t file_open(const uint8_t* filename){
    //   if( (*filename) == (uint8_t)"" || (char)filename[0] == (char)"\n" || filename == NULL){
    //     return -1;
    // }
    dentry_t test_dentry;
    int ret = read_dentry_by_name(filename, &test_dentry);
  return ret;
}

/* file_close()
 * Inputs: void
 * Return Value: 0
 * Function: closes file*/
int32_t file_close(int32_t fd){
  return 0;
}


/* file_write()
 * RTDC function
 * return -1
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes){
  return -1;
}


/* file_read
 * Takes a file name and read that data from it
 * Inputs: takes file name
 * Outputs: return 0 for success
 */
int32_t file_read(int32_t fd, const void* buf, int32_t nbytes){ //get data from pcb
  int32_t curr_pid = get_last_pid();
  // if(curr_pid < -1){
  //   return 0;
  // }
  pcb_t * curr_pcb = (pcb_t*) (_8MEGABYTE - (_8KILOBYTE * (curr_pid +1)));
/************/
  uint32_t inode_address = start_addr + (curr_pcb->file_desc_table[fd].inode_number+1)*FOUR_KB_BLOCK;
  inode_t * curr_inode = (inode_t*)inode_address;

  if(curr_pcb->file_desc_table[fd].file_position > curr_inode->length){ //check if file position is greater than file length
    // printf("file read done\n");
    return 0;
  }
/***********/

  int ret_val;
  ret_val = read_data(curr_pcb->file_desc_table[fd].inode_number, curr_pcb->file_desc_table[fd].file_position, (uint8_t*)buf, nbytes);
  curr_pcb->file_desc_table[fd].file_position = curr_pcb->file_desc_table[fd].file_position + ret_val;
  return ret_val;
}

int32_t dir_open(const uint8_t* filename){
  return 0;
}

/* dir_read()
 * takes data and prints the filename by going through the inodes
 * no inputs/
 * outputs: success or fail (0,-1)
 */
int32_t dir_read(int32_t fd, const void* buf, int32_t nbytes){ //get data from pcb
dentry_t data; //dentry
  int32_t curr_pid = get_last_pid();

  pcb_t * curr_pcb = (pcb_t*) (_8MEGABYTE - (_8KILOBYTE * (curr_pid +1)));

  if (read_dentry_by_index(curr_pcb->file_desc_table[fd].file_position, &data) == 0){
      // size = get_file_size_by_index(data.inode_num);
      curr_pcb->file_desc_table[fd].file_position++;
      int file_name_size;

      for(file_name_size = 0; file_name_size < FILE_NAME_SIZE; file_name_size++){
        if(data.file_name[file_name_size] == NULL){
          break;
        }
      }

      if(nbytes > file_name_size){
        nbytes = file_name_size;
      }
      strncpy((int8_t*)buf, (int8_t*)data.file_name, nbytes);
    
      return nbytes;

  }
 
  else{
    return 0;
  }
}

/*dir_write()
 * rtdc and -1 has to be returned for etc
 * no inputs/
 * outputs 0 is success 1 if fail
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes){ 
  return -1;
}

/*  dir_close()
 *  returns 0
 *  no inputs/outputs
 */
int32_t dir_close(int32_t fd){
  return 0;
}
