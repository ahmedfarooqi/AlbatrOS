/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xff; /* IRQs 0-7  */
uint8_t slave_mask = 0xff;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {

  // unsigned long flags;
  // spinlock_t pic_lock = SPIN_LOCK_UNLOCKED;
  //
  // spin_lock_irqsave(&pic_lock, flags);

  // outb(0xff, 0x21); //mask all interrupts of PIC1
  // outb(0xff, 0xA1); //mask all interrupts of PIC2
  cli();
  outb(0x11, 0x20); //ICW1 - Init i8259-1
  outb(0x20 + 0, 0x21); //ICW2 - Mapping masters interrupts IR0-7 to 0x20-0x27
  outb(0x04, 0x21); //Master has slave at IR2
  outb(0x01, 0x21); //Expect normal EOI

  outb(0x11, 0xA0); //ICW1 - Init i8259-2
  outb(0x20 + 8, 0xA1); //ICW2 - Mapping slaves interrupts IR0-7 to 0x20-0x27
  outb(0x02, 0xA1); //slave is on masters IR2

  outb(0x01, 0xA1);
  sti();
  enable_irq(2);
  // spin_unlock_irqrestore(&pic_lock, flags);

}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {

  if(irq_num >= 0 && irq_num < 8){
    master_mask = master_mask & ~(0x01 << irq_num);
    outb( master_mask, 0x21 );
  }
  else if(irq_num >= 8 && irq_num <= 15){
    slave_mask = slave_mask & ~(0x01 << (irq_num-8));
    outb( slave_mask, 0xA1 );
  }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {

  if(irq_num >= 0 && irq_num < 8){
    master_mask = master_mask | (0x01 << irq_num);
    outb( master_mask, 0x21 );
  }
  else if(irq_num >= 8 && irq_num <= 15){
    slave_mask = slave_mask | (0x01 << (irq_num-8));
    outb( slave_mask, 0xA1 );
  }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
  if(irq_num >= 8 && irq_num <= 15){
		outb(EOI | (irq_num-8), SLAVE_8259_PORT);
    outb(EOI | 2, MASTER_8259_PORT);
  }
  else if(irq_num >= 0 && irq_num < 8)
	  outb(EOI | irq_num, MASTER_8259_PORT);

}
