#include "rtc.h"

#define register_a       0x8A
#define register_b       0x8B
#define register_c       0x8C
#define rtc_irq             8
#define rewrite_port     0x71
#define read_port        0x70
#define mask6b           0x40
#define maskC            0x0C
#define mask7F           0x7F
#define MASKF            0xF
#define MASKF0           0xF0
#define default_freq        2
#define fifteen            15
#define fourteen           14
#define thirteen           13
#define twelve             12
#define eleven             11
#define ten                10
#define nine                9
#define eight               8
#define seven               7
#define six                 6
#define five                5
#define four                4
#define three               3
#define two                 2

volatile int rtc_acknowledge=0;
/*
The 2 IO ports used for the RTC and CMOS are 0x70 and 0x71.
Port 0x70 is used to specify an index or "register number",
and to disable NMI. Port 0x71 is used to read or write from/to
that byte of CMOS configuration space.
Only three bytes of CMOS RAM are used to control the RTC periodic interrupt function.
They are called RTC Status Register A, B, and C.
They are at offset 0xA, 0xB, and 0xC in the CMOS RAM.


For the checkpoint, your OS must execute the test interrupts handler (provided in lib.c)
when an RTC interrupt occurs.
*/

/*
* rtc_intialize
* decription: initializes the rtc
* inputs: none
* outputs: none
*/
void rtc_initialize(){
    outb(register_b, read_port);		// select register B, and disable NMI
    char prev2=inb(rewrite_port);	// read the current value of register B
    outb(register_b, read_port);		// set the index again (a read will reset the index to register D)
    outb((prev2 | mask6b), rewrite_port);	// write the previous value ORed with 0x40. This turns on bit 6 of register

    enable_irq(rtc_irq);
    rtc_open(NULL);

    //rtc_write();
}

/*
* rtc_handler
* decription: is called on interrupts and performs a certain task
* inputs: none
* outputs: none
*/ 
void rtc_handler(){
    //const volatile char param = "1";
    rtc_acknowledge =1;
    //rtc_read(param);
    //test_interrupts();
    outb(maskC, read_port);	// select register C
    inb(rewrite_port);		// throw away contents
    send_eoi(rtc_irq);
}


/*
* rtc_open
* decription: is called in rtc_initialize to set the initial frequency to 2Hz
* inputs: none
* outputs: none
*/
int32_t rtc_open(const uint8_t* filename){
    // if( (*filename) == (uint8_t)"" || (char)filename[0] == (char)"\n" || filename == NULL){
    //     return -1;
    // }


    int default_freq_val = default_freq;
    rtc_write(0,&default_freq_val,0);//set the default frequency
    return 0;
}

/*
* rtc_close
* decription: returns 0
* inputs: none
* outputs: none
*/
int32_t rtc_close(int32_t fd){
    return 0;
}

/*
* rtc_read
* decription: waits on an ackowledge flag and changes it
* inputs: none
* outputs: none
*/
int32_t rtc_read(int32_t fd, const void* buf, int32_t nbytes){
    while(rtc_acknowledge==0){
    }
    //printf("%c",a);
    rtc_acknowledge = 0;
    return 0;
}

/*
* rtc_write
* decription: changes the frequency to the given rate.
* inputs: none
* outputs: none
*/
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){

    int rate; 
    int frequency;
    frequency = *((int32_t *)buf);


    switch (frequency){
        case   2:
            rate = fifteen;
            break;

        case   4:
            rate = fourteen;
            break;

        case   8:
            rate = thirteen;
            break;

        case  16:
            rate = twelve;
            break;

        case  32:
            rate = eleven;
            break;

        case  64:
            rate = ten;
            break;

        case 128:
            rate = nine;
            break;

        case 256:
            rate = eight;
            break;

        case 512:
            rate = seven;
            break;

        case 1024:
            rate = six;
            break;
        // case 2048:
        //     rate = 0x5;

        // case 4096:
        //     rate = 0x4;

        // case 8192:
        //     rate = 0x3;

        default:
            printf("invalid input");
            return -1;
    }


    rate &= MASKF;			// rate must be above 2 and not over 15
    outb(register_a,read_port);		// set index to register A, disable NMI
    char prev=inb(rewrite_port);	// get initial value of register A
    outb(register_a,read_port);		// reset index to A
    outb((prev & MASKF0) | rate,rewrite_port); //write only our rate to A. Note, rate is the bottom 4 bits.


    return 0;
}

