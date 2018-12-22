#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "filesystem.h"
#include "paging.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) &&
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

void paging_test(){
	// uint32_t * x = 0x800000;//b8000
	// uint32_t y = *x;
	// // printf("%d \n", y);
	// putc(y);
}
// void idt_test2(){
// 	int x = 0;
// 	int y = 5;
// 	int z = y/x;
// }
// add more tests here

/* Checkpoint 2 tests */
void cp2rtctest(){
	clear();
	putX(0);putY(0);
	int i,j,k;
	k=1;
	//printf("before for");
	for(i =2;i<=1024;i=i*2){
		//printf("entered for loop");
		clear();
		rtc_write(0,NULL,i);
		//const volatile char param = "1";
		//custom spin lock
		const volatile char param = 0x31;
		for(j=0;j<15*(k);j++){
			//do nothing
			rtc_read(0,NULL,0);
			putc(param);
			if(getX()==79){putc('\n');}
		}
		putX(0);putY(0);
		k++;
	}
	//clear();
}

void cp3rtctest(){
	clear();
	putX(0);putY(0);
	int j;
	const volatile char param = 0x31;
	rtc_write(0,NULL,2);
	for(j=0;j<15;j++){
		//do nothing
		rtc_read(0,NULL,0);
		putc(param);
		if(getX()==79){putc('\n');}
	}
	putX(0);putY(0);
	
	clear();
}

void print_all_file_names(){
	dentry_t data; //dentry
	// int inodes = boot_block->inode_count;
	int i;


	for (i = 0; i<17; i++){
		if(read_dentry_by_index(i, &data) == 0){

			printf("File Name: %s, File Type: %d\n", data.file_name, data.file_type);
			// return 0;
		}
	}

}

void print_file_by_index(uint32_t inode){
	dentry_t data;
	read_dentry_by_index(inode, &data);

	uint8_t buffer[16384];
	int ret_val;
	int k;
	ret_val = read_data(data.inode_num, 0, buffer, 16384);

	for(k = 0; k <4096; k++){
  	printf("%c", buffer[k]);
	}
	printf("\n");

	return;


}

void print_file_by_name(char *file){
	dentry_t data;
	read_dentry_by_name((uint8_t*)file, &data);

	uint8_t buffer[16384];
	int ret_val;
	int k;
	ret_val = read_data(data.inode_num, 0, buffer, 16384);

	for(k = 0; k <4096; k++){
  	printf("%c", buffer[k]);
	}
	printf("\n");
	printf("%s\n", file);
	printf("size: %d\n", ret_val);

	return;
}




/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());
	// paging_test();
	// idt_test2();
	//cp2rtctest();
	//cp3rtctest();
	// clear();
	// print_all_file_names();

	// clear();
	// char * file_name = "fish";
	// print_file_by_name(file_name);

	// print_file_by_index();
}
