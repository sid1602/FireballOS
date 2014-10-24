#ifndef INT_HANDLER 
#define INT_HANDLER

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"

/* Sets up the IDT entries */
extern void setup_idt(void* handler_address, int irq_num);

/* Keyboard Interrupt handler */
void kbd_int_handler();

/* RTC Interrupt handler */
void rtc_int_handler();

/* Dummy Interrupt handler */
void dummy_int_handler();

#endif
