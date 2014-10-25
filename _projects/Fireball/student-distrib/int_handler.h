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

void set_idt();

void set_exception();

void set_interrupt();

void divide_error();

void NMI_Interrupt();

void breakpoint();

void overflow();

void BOUND_range_exceeded();

void invalid_opcode();

void device_not_available();

void double_fault();

void coprocessor_segment_overrun();

void invalid_TSS();

void segment_not_present();

void stack_segment_fault();

void general_protection();

void page_fault();

void floating_point_error();

void alignment_check();

void machine_check();

void floating_point_exception();

#endif
