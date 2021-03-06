#define ASM 	1
kernel_ds:
	.long 0x0018
#include "wrapper.h"
.text

.globl kbd_wrapper, rtc_wrapper, pit_wrapper, mouse_wrapper, jump_to_userspace, execute_syscall, syscall_handler, jump_table, ret_halt

kbd_wrapper:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esp
	pushl %ebp
	pushl %esi
	pushl %edi
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs
	call kbd_int_handler
	popw %gs
	popw %fs
	popw %es
	popw %ds
	popl %edi
	popl %esi
	popl %ebp
	popl %esp
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret
	; pushal
	; call kbd_int_handler
	; popal
	; iret
	
rtc_wrapper:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esp
	pushl %ebp
	pushl %esi
	pushl %edi
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs
	call rtc_int_handler
	popw %gs
	popw %fs
	popw %es
	popw %ds
	popl %edi
	popl %esi
	popl %ebp
	popl %esp
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret

pit_wrapper:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esp
	pushl %ebp
	pushl %esi
	pushl %edi
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs
	call pit_int_handler
	popw %gs
	popw %fs
	popw %es
	popw %ds
	popl %edi
	popl %esi
	popl %ebp
	popl %esp
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret

mouse_wrapper:
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esp
	pushl %ebp
	pushl %esi
	pushl %edi
	pushw %ds
	pushw %es
	pushw %fs
	pushw %gs
	call mouse_int_handler
	popw %gs
	popw %fs
	popw %es
	popw %ds
	popl %edi
	popl %esi
	popl %ebp
	popl %esp
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	iret

jump_to_userspace:
	
 	cli
    movl 4(%esp), %ebx		#have to change to eax since ebx is used in leave and ret 
    mov $0x2B, %ax
    mov %ax, %ds	
                   		
    movl %esp, %eax
    
	pushl $0x2B
    pushl $0x083ffffc		
    pushfl
    popl %eax
    orl $0x200, %eax
    pushl %eax
    pushl $0x23
    pushl %ebx 	
    iret
                   
execute_syscall:   
	PUSHL	%EBX          
	MOVL	$2,%EAX  
	MOVL	8(%ESP),%EBX  
	INT		$0x80         
	POPL	%EBX          
	RET


syscall_handler:
	sti
	pushl %esi
	pushl %edi
	pushl %edx
	pushl %ecx
	pushl %ebx
	cmpl $1, %eax
	jl call_fail
	cmpl $10, %eax
	jg call_fail
	call *jump_table(, %eax, 4)
	jmp end_call
call_fail:
	movl $-1, %eax
end_call:
	popl %ebx
	popl %ecx
	popl %edx
	popl %edi
	popl %esi
	iret
jump_table:
.long 0x0, halt, execute, read, write, open, close, getargs, vidmap
