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
	
  	outb(ICW1, PIC1_COMMAND);  					// starts the initialization sequence (in cascade mode)
	outb(ICW1, PIC2_COMMAND);					// we can even define it as ICW1 instead of the addition

	outb(ICW2_MASTER, PIC1_DATA);               // ICW2: Master PIC vector offset
	outb(ICW2_SLAVE, PIC2_DATA);   				// ICW2: Slave PIC vector offset

	outb(ICW3_MASTER, PIC1_DATA); 				// ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	outb(ICW3_SLAVE, PIC2_DATA);    			// ICW3: tell Slave PIC its cascade identity (0000 0010)
 
	outb(ICW4, PIC1_DATA);
	outb(ICW4, PIC2_DATA);
 
 	outb(a1, PIC1_DATA);   						// restore saved masks.
	outb(a2, PIC2_DATA);

	outb(0XFB, MASTER_8259_PORT + 1);
	outb(0XFF, SLAVE_8259_PORT + 1);
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
    outb(value, port);     
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
    outb(value, port);        
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
		enable_irq(1);
//	enable_irq(0x21);
	//printf(" reached init keyboard ");

//#define KEYBOARD_PORT 0x60
//#define KEYBOARD_STATUS_PORT 0x64                                                                                                                                            
	return;
}

void
init_rtc(void)		
{

	int rate = 0x0F;			// rate must be above 2 and not over 15
	outb(0x8A, 0x70);		// set index to register A, disable NMI
	char prev = inb(0x71);	// get initial value of register A
	outb(0x8A, 0x70);		// reset index to A
	outb((prev & 0xF0) | rate, 0x71); //write only our rate to A. Note, rate is the bottom 4 bits.


	outb(0x8B, 0x70);		// select register B, and disable NMI
	prev = inb(0x71);	// read the current value of register B
	outb(0x8B, 0x70);		// set the index again (a read will reset the index to register D)
	outb(prev | 0x40, 0x71);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
	

	
	enable_irq(8);

//#define KEYBOARD_PORT 0x60
//#define KEYBOARD_STATUS_PORT 0x64
                                                                                                                                             
	return;
}
