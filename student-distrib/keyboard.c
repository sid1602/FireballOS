#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

/* Keyboard Interrupt handler */
/********************************************************
void													*
kbd_int_handler()										*	
*	Inputs:			void								*
*	Outputs:		prints pressed button on screen 	*
*	Return Value:	void								*
*	Function: Prints out character typed to screen		*
********************************************************/
void kbd_int_handler()
{
	disable_irq(1);										//IRQ line was 1 so we get the port numbers
	int to_print;										//disable line so we can complete this before handling some other interrupt 	
//	int shift_flag = 0;
//	int offset = 0;
	//printf("Keyboard INTERRUPT HANDLER");
	
	to_print = inb(0x60);								//store received info into char to check with which key was pressed 
	
//	if(to_print == 0x14)
//	{
//		shift_flag = 1;
//		offset = 32;
//	}	
		
	switch(to_print)									//check which button was pressed on the keyboard
	{
	/*	checking numbers	*/		
		case 0x02:	{	printf("1");	break;	}
		case 0x03:	{	printf("2");	break;	}
		case 0x04:	{	printf("3");	break;	}			
		case 0x05:	{	printf("4");	break;	}
		case 0x06:	{	printf("5");	break;	}
		case 0x07:	{	printf("6");	break;	}
		case 0x08:	{	printf("7");	break;	}
		case 0x09:	{	printf("8");	break;	}	
		case 0x0A:	{	printf("9");	break;	}
		case 0x0B:	{	printf("0");	break;	}
	/*	checking letters	*/	
		case 0x1E:	{	printf("a");	break;	}

//		case 0x1E:	{	printf("%c", 97 - offset);	break;	}				//a		
		case 0x30:	{	printf("b");	break;	}
		case 0x2E:	{	printf("c");	break;	}
		case 0x20:	{	printf("d");	break;	}
		case 0x12:	{	printf("e");	break;	}
		case 0x21:	{	printf("f");	break;	}
		case 0x22:	{	printf("g");	break;	}
		case 0x23:	{	printf("h");	break;	}
		case 0x17:	{	printf("i");	break;	}				
		case 0x24:	{	printf("j");	break;	}
		case 0x25:	{	printf("k");	break;	}
		case 0x26:	{	printf("l");	break;	}
		case 0x32:	{	printf("m");	break;	}
		case 0x31:	{	printf("n");	break;	}
		case 0x18:	{	printf("o");	break;	}
		case 0x19:	{	printf("p");	break;	}
		case 0x10:	{	printf("q");	break;	}
		case 0x13:	{	printf("r");	break;	}		
		case 0x1F:	{	printf("s");	break;	}				
		case 0x14:	{	printf("t");	break;	}
		case 0x16:	{	printf("u");	break;	}
		case 0x2F:	{	printf("v");	break;	}
		case 0x11:	{	printf("w");	break;	}
		case 0x2D:	{	printf("x");	break;	}
		case 0x15:	{	printf("y");	break;	}
		case 0x2C:	{	printf("z");	break;	}
/*	checking special characters	*/	
// 		case 0x0C:	{	printf("-");	break;	}
// 		case 0x0D:	{	printf("=");	break;	}	
// 		case 0x1A:	{	printf("[");	break;	}
// 		case 0x1B:	{	printf("]");	break;	}
// 		case 0x27:	{	printf(";");	break;	}
// 		case 0x28:	{	printf("'");	break;	}
// 		case 0x29:	{	printf("`");	break;	}
// //		case 0x2B:	{	printf("'\'");	break;	}	
// 		case 0x33:	{	printf(",");	break;	}
// 		case 0x34:	{	printf(".");	break;	}
// //		case 0x35:	{	printf("/");	break;	}
// 		case 0x0F:	{	printf("%c", 9);	break;	}
// 		case 0x39:	{	printf(" ");	break;	}
// //		case 0x0E:	{	printf("");	break;	}

	}
	send_eoi(1);										//inform when finished handling interrupt
	enable_irq(1);										//unmask so that new interrupts can be taken care of
}


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

/* Dummy Interrupt handler */
/********************************************************
void													*
dummy_int_handler()										*	
*	Inputs:			void								*
*	Outputs:		for system calls			 	 	*
*	Return Value:	void								*
*	Function: Should execute test_interrupts handler	*
********************************************************/
/* Prints int # */
void dummy_int_handler()
{
	printf("DUMMY INTERRUPT HANDLER");
}
