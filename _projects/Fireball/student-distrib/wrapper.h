#ifndef _WRAPPER_H
#define _WRAPPER_H

#ifndef ASM

#include "types.h"

void kbd_wrapper();

void rtc_wrapper();

void pit_handler();

void syscall_handler();

void jump_to_userspace(uint32_t entry_addr);

void jump_to_userspace_again(uint32_t entry_addr);

#endif /* ASM */

#endif /* _WRAPPER_H */
