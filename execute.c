#include "execute.h"
 
#define FOURMEGABYTE 0x400000
#define MASK4MBRW   0x83
#define ELF1 0x7f
#define ELF2 0x45
#define ELF3 0x4c
#define ELF4 0x46
#define _128MEGABYTE    0x8000000
#define _8MEGABYTE      0x800000
#define _4MEGABYTE      0x400000
#define _8KILOBYTE      0x2000
#define PROGRAMIMAGE    0x08048000
#define DS              0x18
#define USER_SS 0x83FFFFC
#define RTC 0 
#define DIR 1 
#define REG_FILE 2
#define MAX_PROCESSES 8
#define LEN_FDT 7
#define FDT_LOOP_BEGIN 2
#define FDT_LOOP_END 8
#define FILE_SIZE_LIMIT 32
#define BUFFER_SIZE 128
#define BUF_4  4
#define _16KILOBYTE     16384
#define VIDEOMEMSTART 0xB8000
//use process id instead of process count
uint32_t process_array[MAX_PROCESSES] = {0,0,0,0,0,0,0,0};
 
uint8_t process_count = 0;
// volatile operations_t  rtc_jump = {rtc_open, rtc_close, rtc_read, rtc_write};
// volatile operations_t  file_jump = {file_open, file_close, file_read, file_write};
// volatile operations_t  dir_jump = {dir_open, dir_close, dir_read, dir_write};
// volatile operations_t  stin_jump = {terminal_open, terminal_close, terminal_read, ret_neg_one};
// volatile operations_t  stout_jump = {terminal_open, terminal_close, ret_neg_one, terminal_write};
// volatile operations_t  empty_jump = {ret_neg_one, ret_neg_one, ret_neg_one, ret_neg_one};
//attemps to load and execute new program, handing off the processor to the new program unti it terminates
//command is a space seperated sequence of words
    //first word is the file name, the rest of the words will be commands that are retrieved by getargs syscall
//returns -1 if cannot be executed (program DNE or filename is not an executable)
    //return 256 if program dies by exception
    //return value in range of 0 - 255 if program executes halt (return value of halt returns from execute)

/* ret_neg_one
 * Inputs: void
 * Return Value: -1
 * Function: helper function to return -1*/
uint32_t ret_neg_one(){
    return -1;
}

int32_t ret_neg_one_open(const uint8_t* filename){
    return -1;
}


int32_t ret_neg_one_close(int32_t fd){
    return -1;
}

int32_t ret_neg_one_read(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

int32_t ret_neg_one_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/*  Function: syscall_read
 *  Description: system call function to read from different kinds of file
 *  Inputs: Pointer to the buffer and size of buffer
 *  Outputs: number of bytes read
 */
int32_t syscall_read (int32_t fd, void* buf, int32_t nbytes){
    // printf("reached read\n");
    // fd = 2;
    int32_t curpid =  get_last_pid();
    pcb_t * pcb_curr = (pcb_t*)(_8MEGABYTE - (_8KILOBYTE * (curpid+1)));
    // printf("current term: %d", pcb_curr->process_id);

    if (fd<0 || fd>LEN_FDT || buf == NULL || pcb_curr->file_desc_table[fd].flags == 0){
        return -1;
    }

    return pcb_curr->file_desc_table[fd].op_table.read(fd,buf,nbytes);
}
/*  Function: syscall_write
 *  Description: system call function to write to different kinds of file
 *  Inputs: Pointer to the buffer and size of buffer
 *  Outputs: number of bytes written
 */
int32_t syscall_write (int32_t fd, const void* buf, int32_t nbytes){
    int32_t curpid =  get_last_pid();
    if (fd<0 || fd>LEN_FDT){
        return -1;
    }
    pcb_t * pcb_curr = (pcb_t*)(_8MEGABYTE - (_8KILOBYTE * (curpid+1)));

    return pcb_curr->file_desc_table[fd].op_table.write(fd,buf,nbytes);
}
/*  Function: syscall_open
 *  Description: system call function to open different kinds of file
 *  Inputs: filename to open
 *  Outputs: available index of file descriptor
 */
int32_t syscall_open (const uint8_t* filename){

    if( (*filename) == (uint8_t)"" || (char)filename[0] == (char)"\n" || filename == NULL || *filename == NULL){
        return -1;
    }


    int32_t curpid =  get_last_pid();

    dentry_t check_dentry;
    if(read_dentry_by_name((uint8_t*)filename, &check_dentry) == -1){ //if file not found in bootblock
        return -1; //return failure
    }
    //check for pcb full
    uint32_t availability_flag=0 ;//assume no space available
    uint32_t available_spot=0;

    uint32_t i;
    pcb_t * pcb_curr = (pcb_t*)(_8MEGABYTE - (_8KILOBYTE * (curpid+1)));

    for(i = FDT_LOOP_BEGIN; i < FDT_LOOP_END; i++){
        if(pcb_curr->file_desc_table[i].flags==0){ //if fdn is empty
            pcb_curr->file_desc_table[i].flags=1;
            pcb_curr->file_desc_table[i].file_position = 0x0;//start of file
            availability_flag=1; // we have found available spot
            available_spot=i;
            break;
        }
    }
    if(availability_flag==0){ //if no available spot found
        return -1; // return failure
    }
    if(check_dentry.file_type == RTC){
        // pcb_curr->file_desc_table[available_spot].op_table =  rtc_jump;
        pcb_curr->file_desc_table[available_spot].op_table.open =  rtc_open;
        pcb_curr->file_desc_table[available_spot].op_table.close =  rtc_close;
        pcb_curr->file_desc_table[available_spot].op_table.read =  rtc_read;
        pcb_curr->file_desc_table[available_spot].op_table.write =  rtc_write;
        pcb_curr->file_desc_table[available_spot].inode_number = NULL;
    }

    else if(check_dentry.file_type == DIR){
        // pcb_curr->file_desc_table[available_spot].op_table =  dir_jump;
        pcb_curr->file_desc_table[available_spot].op_table.open =  dir_open;
        pcb_curr->file_desc_table[available_spot].op_table.close =  dir_close;
        pcb_curr->file_desc_table[available_spot].op_table.read =  dir_read;
        pcb_curr->file_desc_table[available_spot].op_table.write =  dir_write;
        pcb_curr->file_desc_table[available_spot].inode_number = NULL;
    }

    else if(check_dentry.file_type == REG_FILE){
        // pcb_curr->file_desc_table[available_spot].op_table =  file_jump;
        pcb_curr->file_desc_table[available_spot].op_table.open =  file_open;
        pcb_curr->file_desc_table[available_spot].op_table.close =  file_close;
        pcb_curr->file_desc_table[available_spot].op_table.read =  file_read;
        pcb_curr->file_desc_table[available_spot].op_table.write =  file_write;
        pcb_curr->file_desc_table[available_spot].inode_number = check_dentry.inode_num;
    }

    pcb_curr->file_desc_table[available_spot].op_table.open((uint8_t*)filename);

    return (int32_t)available_spot;
}
/*  Function: syscall_close
 *  Description: system call function to close different kinds of file
 *  Inputs: Pointer to the buffer and size of buffer
 *  Outputs: successful closing or not
 */
int32_t syscall_close (int32_t fd){
    int32_t curpid =  get_last_pid();
    if(curpid == 0 || curpid == -1){return -1;}
    if (fd<2 || fd>LEN_FDT ){
        return -1;
    }
    pcb_t * pcb_curr = (pcb_t*)(_8MEGABYTE - (_8KILOBYTE * (curpid+1)));
    fdt_t fdt_temp = pcb_curr->file_desc_table[fd];
    if(fdt_temp.flags == 0){return -1;}
    pcb_curr->file_desc_table[fd].flags = 0;
    operations_t operations_temp = fdt_temp.op_table;
    if(operations_temp.close==NULL || operations_temp.close==0){
        return -1;
    }
    int32_t temp = operations_temp.close(fd);
    return temp;
}
/*  Function: syscall_halt
 *  Description: system call function to halt a program
 *  Inputs: Error code that invokes halt
 *  Outputs: Always returns 0
 */
int32_t syscall_halt (uint8_t status){
    // cli();
    // printf("pre halt: ");putc(process_array[0]);putc(process_array[1]);putc(process_array[2]);putc(process_array[3]);putc(process_array[4]);putc(process_array[5]);putc(process_array[6]);putc(process_array[7]);putc('\n');

    // int32_t curr_pid = get_last_pid();//add pid to pcb struct + global cur pcb pointer
    int32_t curr_pid = global_terminal_array[active_terminal].top_pcb->process_id;
    pcb_t* curr_pcb = global_terminal_array[active_terminal].top_pcb;


    if(curr_pid == -1){
        printf("error\n");
        return -1;
    }
    if(curr_pcb->parent_id == -1 ){//call halt on root shell        
        process_array[curr_pid] = 0;
        syscall_execute((uint8_t*)"shell");
        return -1;
    }
    // pcb_t * pcb_curr = (pcb_t*)(_8MEGABYTE - (_8KILOBYTE * (curr_pid+1)));
    
    // pcb_t * pcb_parent = (pcb_t*)(_8MEGABYTE - (_8KILOBYTE * (pcb_curr->parent_id)));
    int i;
    for(i = 2; i < FDT_LOOP_END; i++){
        if(curr_pcb->file_desc_table[i].flags == 1){
            syscall_close(i);
        }
        curr_pcb->file_desc_table[i].flags = 0;
        curr_pcb->file_desc_table[i].inode_number = 0;
        curr_pcb->file_desc_table[i].file_position = 0;
        // pcb_curr->file_desc_table[i].op_table = empty_jump;
    }
    process_array[curr_pid] = 0; //to be done at the end
    tss.esp0  =(uint32_t)(_8MEGABYTE - (_8KILOBYTE * curr_pcb->parent_id)-4);
    add_page_directory(_128MEGABYTE, _8MEGABYTE + (curr_pcb->parent_id * _4MEGABYTE));
    // printf("post halt: ");putc(process_array[0]);putc(process_array[1]);putc(process_array[2]);putc(process_array[3]);putc(process_array[4]);putc(process_array[5]);putc(process_array[6]);putc(process_array[7]);putc('\n');
    global_terminal_array[active_terminal].top_pcb = (pcb_t*)(_8MEGABYTE - (_8KILOBYTE * (curr_pcb->parent_id+1)));
    asm volatile("                      \n\
            movl %0, %%eax              \n\
            movl %1, %%esp              \n\
            movl %2, %%ebp              \n\
            jmp REVERSE_CONTEXT_SWITCH  \n\
            "
            :
            :"r"((uint32_t)status), "r"(curr_pcb->parent_esp), "r"(curr_pcb->parent_ebp)
            : "eax"
            );

    return 0;
}
/*  Function: syscall_execute
 *  Description: system call function to execute kernel code at privilege level 0 and user code at level 3
 *  Inputs: command is a keyboard input
 *  Outputs: on failure returns -1, on success goes to context switch
 */
uint32_t syscall_execute(const uint8_t * command){
    // cli();
    // printf("pre exec: ");putc(process_array[0]);putc(process_array[1]);putc(process_array[2]);putc(process_array[3]);putc(process_array[4]);putc(process_array[5]);putc(process_array[6]);putc(process_array[7]);
    if(command == NULL || (char)*command == (char)""){
        return -1;
    }
    uint32_t next_pid = get_pid();
    if(next_pid == -1 || get_last_pid() == 6 || process_array[5]==1){
        printf("full processess\n");
        return -1;
    }
    process_count = next_pid;
    int i=0;
    int name_size = 0;

    /**********************/
    /*****Parse command****/
    uint8_t filename[FILE_SIZE_LIMIT] = {'\0'};//32 is the limit for the file size CHANGE MAGIC NUMBER
    while( command[i] != '\0'  || command[i] == '\n' || command[i]!= ' '){
        if (command[i] == ' '){
            break;
        }
        filename[i] = command[i];
        name_size++;
        i++;    //should probably have some kind of check here for filesize
        if(i == FILE_SIZE_LIMIT-1){
            break; // when the file name is too big
        }
    }
    // printf("| %c%c%c%c%c |\n", command[0], command[1], command[2], command[3], command[4]); 
    i++;// the space should be skipped

    uint32_t arg_size = 0; // NEEDS TO BE CHANGED NOT 123
    uint8_t arg_buf[BUFFER_SIZE] = {'\0'}; //come back to change size

    while( (char)command[i] != NULL || (char)command[i]=='\n'){
        arg_buf[arg_size] = (char)command[i];
        i++;
        arg_size++;
        if(arg_size ==BUFFER_SIZE){
            printf("Args too long\n");
            return -1;//if the arguemnt size exceed the buffer
        }
    }

    int a;

    for(a = arg_size; a < BUFFER_SIZE ; a++){ //Nulls for indexes after arg_size
        arg_buf[a] = NULL;
    }
    // printf("| %c%c%c%c |\n", arg_buf[0], arg_buf[1], arg_buf[2], arg_buf[3]);

    int b;

    for( b = name_size; b < FILE_NAME_SIZE; b++){
        filename[b] = NULL;
    }

    /**********************/
    /***executable check***/
    dentry_t data;
    int exist = read_dentry_by_name((uint8_t*)filename, &data); //checking if file exists

    if(exist == -1){//if file dne
        return -1;
    }

    uint8_t buffer[BUF_4];
    uint32_t read_retval;
    read_retval = read_data(data.inode_num, 0, buffer, BUF_4);

    if(read_retval != BUF_4){ // Failed to read 4 bytes
        return -1;
    }
    //checking if first 3 chars are ELF, if so then it is executable
    if( (uint8_t)buffer[0] != ELF1 || (uint8_t)buffer[1] != ELF2 ||
        (uint8_t)buffer[2] != ELF3 || (uint8_t)buffer[3] != ELF4 ) {
            return -1;
    }

    /**********************/
    /***Paging setup***/
    // sti();
    add_page_directory(_128MEGABYTE, _8MEGABYTE + (next_pid * _4MEGABYTE) );


    /**********************/
    /*Copying data into memory*/
    uint32_t ret;
    ret = read_data(data.inode_num, 0, (uint8_t *)PROGRAMIMAGE, _16KILOBYTE);
    //address that has the data of the particular file
    uint32_t entry_point; //variable that would hold the entry point
    uint32_t entry_point_read_check;
    entry_point_read_check = read_data(data.inode_num, 24, (uint8_t*)&(entry_point), BUF_4);  //reading bytes 24-27 of the file for entry point
    if(entry_point_read_check != BUF_4){
        printf("Get Entry point failed\n");
        return -1;
    }


  /**********************/
  /*****PCB setup********/
    pcb_t * curr_pcb = (pcb_t*)(_8MEGABYTE - (_8KILOBYTE * (next_pid+1)));
    curr_pcb->process_id = next_pid;                                        //its stack in the kernel page
    if(next_pid == 0 /* || next_pid == 1 || next_pid == 2 */){
        curr_pcb->parent_id = -1;
        curr_pcb->terminal_number = 0;

    }
    else if(t2_launch == 2){
        // curr_pcb->parent_id = global_terminal_array[visible_terminal].top_pcb->process_id;
        curr_pcb->terminal_number = 1;
        curr_pcb->parent_id = -1;
        t2_launch =  3;
    }
    else if(t3_launch == 2){
            curr_pcb->terminal_number = 2;
            curr_pcb->parent_id =  -1;
            t3_launch = 3;
    }
    else{
        curr_pcb->parent_id = global_terminal_array[active_terminal].top_pcb->process_id;
        asm volatile("  \n\
            movl %%ebp, %0 \n\
            movl %%esp, %1 \n\
        "
        :"=g"(curr_pcb->parent_ebp), "=g"(curr_pcb->parent_esp)
        );
    }
    int j = 0;
    // visible_terminal++;
    // visible_terminal%=3;
    for(j = 2; j < FDTSIZE; j++){
        curr_pcb->file_desc_table[i].flags = 0;
        curr_pcb->file_desc_table[i].op_table.open = ret_neg_one_open;
        curr_pcb->file_desc_table[i].op_table.close = ret_neg_one_close;
        curr_pcb->file_desc_table[i].op_table.read = ret_neg_one_read;
        curr_pcb->file_desc_table[i].op_table.write = ret_neg_one_write;
    }
    int tempvar = 1;
    if(tempvar == 1){
        curr_pcb->file_desc_table[0].op_table.open = terminal_open;
        curr_pcb->file_desc_table[0].op_table.close = terminal_close;
        curr_pcb->file_desc_table[0].op_table.read = terminal_read;
        curr_pcb->file_desc_table[0].op_table.write = ret_neg_one_write;
    
        curr_pcb->file_desc_table[1].op_table.open = terminal_open;
        curr_pcb->file_desc_table[1].op_table.close = terminal_close;
        curr_pcb->file_desc_table[1].op_table.read = ret_neg_one_read;
        curr_pcb->file_desc_table[1].op_table.write = terminal_write;

	    curr_pcb->file_desc_table[0].flags = 1;
    	curr_pcb->file_desc_table[1].flags = 1;
    }

    strncpy((int8_t*)curr_pcb->arguments,(int8_t*)arg_buf, BUFFER_SIZE); //Args to be used in getargs system call

    int k = 0; //Clearing the arg buff
    for(k = 0; k < BUFFER_SIZE; k++){
        arg_buf[k] = NULL;
    }



    //context switch
    tss.esp0 =(uint32_t) (_8MEGABYTE - (_8KILOBYTE * (next_pid))-BUF_4);
    tss.ss0 = (uint32_t) DS;
    process_array[next_pid]=1;
    
    global_terminal_array[active_terminal].top_pcb = curr_pcb;
    global_terminal_array[active_terminal].curr_esp0 = tss.esp0;
    global_terminal_array[active_terminal].curr_ss0 = tss.ss0;
    // printf("post exec: ");putc(process_array[0]);putc(process_array[1]);putc(process_array[2]);putc(process_array[3]);putc(process_array[4]);putc(process_array[5]);putc(process_array[6]);putc(process_array[7]);
    return (uint32_t)CONTEXT_SWITCH(entry_point);
}

/*get_pid
 *returns  the last free index into the process array
 * we use this index to calculate the offset for storying the pcb in memory
 * Inputs: none
 * Ouputs: none
 */
int32_t get_pid(){
    int i = 0;
    for(i = 0; i < MAX_PROCESSES-1; i++){
        if(process_array[i] == 0){
            return i;
        }
    }
    return -1;
}

/*get_last_pid
 *returns  the last free occupied pcb in the process array
 * we use this index to calculate the offset for getting the pcb in memory
 * Inputs: none
 * Ouputs: none
 */
int32_t get_last_pid(){
    int i = 0;
    for(i = 1; i < MAX_PROCESSES-1; i++){
        if(process_array[i] == 0){
            return i-1;
        }
    }

    return -1;
}

/*
*getargs
* reads the program’s command line arguments into a user-level buffer
* Returns -1 if there are no arguments, or if the arguments don't fit in the buffer
*/
int32_t syscall_getargs (uint8_t* buf, int32_t nbytes){
  if(nbytes < 1 || buf == NULL){
    // printf("getargs called -1\n");
    return -1;
  }
//   printf("getargs called 0\n");
  int32_t curpid =  get_last_pid();
  pcb_t * pcb_curr = (pcb_t*)(_8MEGABYTE - (_8KILOBYTE * (curpid+1)));
  printf("%s\n", pcb_curr->arguments);
//   char test_str[] = "frame0.txt";
  strncpy((int8_t*)buf,(int8_t*)pcb_curr->arguments, nbytes);
//   strncpy((int8_t*)buf, test_str, 10);
  return 0;

}
/*syscall_vidmap
 * maps the text-mode video memory into user space at pre-set virtual address
 * fails to allocate if located in the initial 4MB page
 * inputs: the screen_start is the address to be mapped
 * outputs: return value if successfully allocated
 */
int32_t syscall_vidmap(uint8_t** screen_start){
    if( (uint32_t)screen_start > (FOURMEGABYTE+_128MEGABYTE) || (uint32_t)screen_start < _128MEGABYTE ){//if parameters are invaliid
        return -1;
        // printf("fail vidmap\n");
    }
    //we pick 132MB because this is first free address after the program miage in memory
    add_vid_directory( (uint32_t)(_128MEGABYTE + _4MEGABYTE) , (uint32_t)multi[active_terminal]);//map to video mem
    *screen_start = (uint8_t *)(_128MEGABYTE + _4MEGABYTE);
    return 0;//return success
}
