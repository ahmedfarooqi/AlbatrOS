#include "idt.h"
#include "x86_desc.h"

void div_zero(){
  printf("divide error\n");
  while(1);
  return;
}

void reserved(){
  printf("reserved\n");
  while(1);
  return;
}

void nmi(){
  printf("nmi\n");
  while(1);
  return;
}

void breakpoint(){
  printf("breakpoint\n");
  while(1);
  return;
}

void overflow(){
  printf("overflow\n");
  while(1);
  return;
}

void bounds_range(){
  printf("bounds_range\n");
  while(1);
  return;
}

void invalid_opcode(){
  printf("invalid_opcode\n");
  while(1);
  return;
}

void device_not_available(){
  printf("device_not_available\n");
  while(1);
  return;
}

void double_fault(){
  printf("double_fault\n");
  while(1);
  return;
}

void coprocessor_segment(){
  printf("coprocessor_segment\n");
  while(1);
  return;
}

void invalid_TSS(){
  printf("invalid_TSS\n");
  while(1);
  return;
}

void segment_not_present(){
  printf("segment_not_present\n");
  while(1);
  return;
}

void stack_seg_fault(){
  printf("stack_seg_fault\n");
  while(1);
  return;
}

void general_protection_fault(){
  printf("general_protection_fault\n");
  while(1);
  return;
}

void page_fault(){
  printf("page_fault\n");
  while(1);
  return;
}

void math_fault(){
  printf("math_fault\n");
  while(1);
  return;
}

void alignment_check(){
  printf("alignment_check\n");
  while(1);
  return;
}

void machine_check(){
  printf("machine_check\n");
  while(1);
  return;
}

void floating_pt_exception(){
  printf("floating_pt_exception\n");
  while(1);
  return;
}

void user_defineable(){
  printf("user_defineable\n");
  while(1);
  return;
}

void filling(){
  // printf("filling\n");
  while(1);
  return;
}

void keyboard(){
  printf("keyboard\n");
  while(1);
  return;
}



void init_idt(){

  // SET_IDT_ENTRY(idt[0], div_zero);
  int i;
  for(i=0; i<NUM_VEC; i++){
    if(i < 0x20){
      idt[i].seg_selector = KERNEL_CS;
      idt[i].reserved4 = 0;
      idt[i].reserved3 = 0;
      idt[i].reserved2 = 1;
      idt[i].reserved1 = 1;
      idt[i].size = 1;
      idt[i].reserved0 = 0;
      idt[i].dpl = 0;
      idt[i].present = 1;
    }
    else if((i >= 0x20 && i <= 0x2f) || i == 0x80){
      idt[i].seg_selector = KERNEL_CS;
      idt[i].reserved4 = 0;
      idt[i].reserved3 = 1;
      idt[i].reserved2 = 1;
      idt[i].reserved1 = 1;
      idt[i].size = 1;
      idt[i].reserved0 = 0;
      idt[i].dpl = 0;
      idt[i].present = 1;
    }

  }



  SET_IDT_ENTRY(idt[0], div_zero);
  SET_IDT_ENTRY(idt[1], reserved);
  SET_IDT_ENTRY(idt[2], nmi);
  SET_IDT_ENTRY(idt[3], breakpoint);
  SET_IDT_ENTRY(idt[4], overflow);
  SET_IDT_ENTRY(idt[5], bounds_range);
  SET_IDT_ENTRY(idt[6], invalid_opcode);
  SET_IDT_ENTRY(idt[7], device_not_available);
  SET_IDT_ENTRY(idt[8], double_fault);
  SET_IDT_ENTRY(idt[9], coprocessor_segment);
  SET_IDT_ENTRY(idt[10], invalid_TSS);
  SET_IDT_ENTRY(idt[11], segment_not_present);
  SET_IDT_ENTRY(idt[12], stack_seg_fault);
  SET_IDT_ENTRY(idt[13], general_protection_fault);
  SET_IDT_ENTRY(idt[14], page_fault);
  SET_IDT_ENTRY(idt[15], reserved);
  SET_IDT_ENTRY(idt[16], math_fault);
  SET_IDT_ENTRY(idt[17], alignment_check);
  SET_IDT_ENTRY(idt[18], machine_check);
  SET_IDT_ENTRY(idt[19], floating_pt_exception);
  SET_IDT_ENTRY(idt[20], reserved);

  // if(i==0x21){
  //   SET_IDT_ENTRY(idt[i], keyboard_handler);
  // }
  // SET_IDT_ENTRY(idt[0x21], keyboard_handler);


  for(i = 21; i < 256; i++){

    if(i==0x21){
      SET_IDT_ENTRY(idt[i], keyboard_handler);
    }

    else if(i<0x20){
      SET_IDT_ENTRY(idt[i], reserved);
    }

    else if( i >= 0x20 && i >= 0x2f){
      SET_IDT_ENTRY(idt[i], user_defineable);
    }

    else{
      SET_IDT_ENTRY(idt[i], filling);
    }

  }

  SET_IDT_ENTRY(idt[0x21], keyboard_handler_assembly);
  SET_IDT_ENTRY(idt[0x28], rtc_handler_assembly);
  SET_IDT_ENTRY(idt[0x20], pit_handler_assembly);

  idt[0x80].dpl = 3;//set the descriptor privelege level for the system call handler
  idt[0x80].present = 1; //these are not magic numbers
  SET_IDT_ENTRY(idt[0x80], syscall_handler_assembly);

}
