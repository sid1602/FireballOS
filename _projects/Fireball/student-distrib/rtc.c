#include "rtc.h"
#include "lib.h"

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
	printf("RTC INTERRUPT HANDLER");
	test_interrupts();									//calls function that writes video memory to screen.
}
