#define ASM 1

.globl CONTEXT_SWITCH
.globl REVERSE_CONTEXT_SWITCH

CONTEXT_SWITCH:
     movl 4(%esp), %ebx # Getting entry point
     movw $0x2B, %ax # permission settings for context switching
     movw %ax, %ds      
     pushl $0x2B          
     pushl $0x83FFFFC             
     pushfl               
     popl %edx           
     orl $0x200, %edx 	  
     pushl %edx          
     pushl $0x0023       # user ds     
     pushl %ebx     
     sti   
     iret                 
REVERSE_CONTEXT_SWITCH:
     leave
     ret
