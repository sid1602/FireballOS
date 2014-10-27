#include "rtc.h"
#include "lib.h"
#include "i8259.h"

/* RTC Interrupt handler */
/********************************************************
void													*
rtc_int_handler()										*	
*	Inputs:			void								*
*	Outputs:		writes video memory to screen 	 	*
*	Return Value:	void								*
*	Function: Should execute test_interrupts handler	*
********************************************************/
void rtc_int_handler()
{
	disable_irq(8);
	//printf("RTC INTERRUPT HANDLER");
	test_interrupts();									//calls function that writes video memory to screen.
	send_eoi(8);
	outb(0x0C, 0x70);									// select register C
	inb(0x71);											// just throw away contents
	enable_irq(8);
}
