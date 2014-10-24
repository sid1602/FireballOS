#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "int_handler.h"




/*void setup_idt(void* handler_address, int irq_num)
{
uint32_t idt_upper;
uint32_t idt_lower;
// Set Offset 31:16 Present DPL 
idt_upper = (handler_address & 0xffff0000) | (1 << 15) | (3 << 13) | 0x0E00; // Size, other ‘1’ bits 
// Segment Selector Offset 15:00 
idt_lower = (KERNEL_CS << 16) | handler_address & 0xffff;
// Fill in the entry in the IDT 
idt[32+irq_num] = ((unsigned long long)(idt_upper) << 32) | idt_lower;
}
*/


/* Keyboard Interrupt handler */
/* Prints out character typed to screen */
void kbd_int_handler()
{
	char to_print;
	//get this char somehow
	printf("Keyboard INTERRUPT HANDLER");
	putc(to_print);
}

/* RTC Interrupt handler */
/* Should execute test_interrupts handler */
void rtc_int_handler()
{
	printf("RTC INTERRUPT HANDLER");
	test_interrupts();
}

/* Dummy Interrupt handler */
/* Prints int # */
void dummy_int_handler()
{
	printf("DUMMY INTERRUPT HANDLER");

}
