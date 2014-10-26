#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#ifndef ASM	
#include "wrapper.h"

extern void kbd_int_handler();

extern void rtc_int_handler();

extern void dummy_int_handler();

#endif	/* ASM */

#endif 	/* KEYBOARD */

