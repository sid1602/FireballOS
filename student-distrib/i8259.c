/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"


//////////////////	WRITTEN BY ME 	/////////////////////////////////////////////////////////////////////////

#define PIC1			0x20		/*IO base address for master PIC*/
#define PIC2			0xA0		/*IO base address for slave PIC */
#define PIC1_COMMAND	PIC1 		
#define PIC1_DATA		(PIC1+1) 	/*This is basically port 0x21	*/
#define PIC2_COMMAND	PIC2
#define PIC2_DATA		(PIC2+1) 	/*This is basically port 0xA1	*/
#define PIC_EOI			0x20 		/*End-of-interrupt command code	*/



#define ICW1_INIT	0x10		/* Initialization - required! */
#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */

//some extra definations 
// #define ICW1 		0x11
// #define ICW2_MASTER	0x21
// #define ICW2_SLAVE	0x28
// #define ICW3_MASTER	0x04
// #define ICW3_SLAVE	0x02
// #define ICW4 		0x01
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{
	 unsigned char a1;
	 unsigned char a2;
 
	 a1 = inb(PIC1_DATA);                        // save masks
	 a2 = inb(PIC2_DATA);
 
  	outb(PIC1_COMMAND, ICW1);  					// starts the initialization sequence (in cascade mode)
	outb(PIC2_COMMAND, ICW1);					// we can even define it as ICW1 instead of the addition

	outb(PIC1_DATA, ICW2_MASTER);               // ICW2: Master PIC vector offset
	outb(PIC2_DATA, ICW2_SLAVE);   				// ICW2: Slave PIC vector offset

	outb(PIC1_DATA, ICW3_MASTER); 				// ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(PIC2_DATA, ICW3_SLAVE);    			// ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(PIC1_DATA, ICW4);
	outb(PIC2_DATA, ICW4);
 
 	outb(PIC1_DATA, a1);   						// restore saved masks.
	outb(PIC2_DATA, a2);
/*	There are some wait statemets which come below outb commands. Refer to http://wiki.osdev.org/8259_PIC for exact code and some of the code is below*/

	//io_wait();			in case you want to use it, #include <io.h>
	/*io_wait() checks the descriptors that the program is interested in to see whether any of them are ready. If none of them are ready, io_wait() tries to pause until one of them is ready, so that it does not take time away from other programs running on the same computer.
	io_wait pays attention to timeouts: if a descriptor reaches its timeout, and the program is interested in reading or writing that descriptor, io_wait will return promptly.

	Under some circumstances, io_wait will return even though no interesting descriptors are ready. Do not assume that a descriptor is ready merely because io_wait has returned.

	io_wait is not interrupted by the delivery of a signal. Programs that expect interruption are unreliable: they will block if the same signal is delivered a moment before io_wait. The correct way to handle signals is with the self-pipe trick.
*/
//	printf(" MURALI WANTS SO MUCH MO ");
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
	uint16_t port;
    uint8_t value;
 
    if(irq_num < 8)
    	port = PIC1_DATA;

    else
    {
        port = PIC2_DATA;
        irq_num -= 8;
    }

    value = inb(port) & ~(1 << irq_num);
    outb(port, value);     
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
	uint16_t port;
    uint8_t value;
 
    if(irq_num < 8)
    	port = PIC1_DATA;
    
    else
    {
        port = PIC2_DATA;
        irq_num -= 8;
    }

    value = inb(port) | (1 << irq_num);
    outb(port, value);        
}	

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
		if(irq_num >= 8)
	{
		uint32_t temp_irq_num = irq_num - 8;
		unsigned char temp_eoi = EOI | temp_irq_num; 
		outb(temp_eoi, PIC2);
 		outb(EOI | 2, PIC1);		
	}	
	else
	{
		unsigned char temp_eoi = EOI | irq_num;
		outb(temp_eoi, PIC1);
	}	
}

void
init_keyboard(void)		
{
	enable_irq(0x21);
	
//#define KEYBOARD_PORT 0x60
//#define KEYBOARD_STATUS_PORT 0x64                                                                                                                                            
	return;
}

void
init_rtc(void)		
{
	enable_irq(0x28);
	
//#define KEYBOARD_PORT 0x60
//#define KEYBOARD_STATUS_PORT 0x64
                                                                                                                                             
	return;
}


//////////////////////////aksdhflaksjdhflaksjdhflakjdhflajdhflaskjdf SIDDHARTH
//void kbd_int_handler()
//{
//	char typed;
//	inb(0x21, typed);
//}

//////////@#$%^&*(&^%&^#*^$*&^%(%(*^*%^@)* OEFUI#&* #r))

