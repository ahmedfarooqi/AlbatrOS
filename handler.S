#define ASM     1


.globl keyboard_handler_assembly
.align 4

.globl rtc_handler_assembly
.align 4

.globl pit_handler_assembly
.align 4

.globl syscall_handler_assembly
.align 4


keyboard_handler_assembly:
  pushfl
  pushal
  pushl %eax
  pushl %ecx
  pushl %edx
  call keyboard_handler
  popl %edx
  popl %ecx
  popl %eax
  popal
  popfl
  iret

rtc_handler_assembly:
  pushfl
  pushal
  pushl %eax
  pushl %ecx
  pushl %edx
  call rtc_handler
  popl %edx
  popl %ecx
  popl %eax
  popal
  popfl
  iret

pit_handler_assembly:
  pushfl
  pushal
  pushl %eax
  pushl %ecx
  pushl %edx
  call pit_handler
  popl %edx
  popl %ecx
  popl %eax
  popal
  popfl
  iret

syscall_handler_assembly: # eax should not be saved
  # cli
  pushfl
  pushl %es
  pushl %ds
  pushl %ebp
  pushl %edi
  pushl %esi
  pushl %edx
  pushl %ecx
  pushl %ebx


  cmpl $8, %eax
  jg default
  cmpl $1,%eax
  jl default
#  subl $1, %eax 
  # sti
  call *calltable(,%eax,4)
  # cli
  popl %ebx
  popl %ecx
  popl %edx
  popl %esi
  popl %edi
  popl %ebp
  popl %ds
  popl %es
  popfl
  # sti
  iret
  



 calltable:
    .long 0x0, syscall_halt, syscall_execute, syscall_read, syscall_write, syscall_open, syscall_close, syscall_getargs, syscall_vidmap



default: 
    movl $-1, %eax
    
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popl %ds
    popl %es
    popfl
    sti
    iret
