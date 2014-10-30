#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "keyboard.h"


int interrupt_number = 0;
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
	interrupt_number++;
	send_eoi(8);
	outb(0x0C, 0x70);									// select register C
	inb(0x71);											// just throw away contents
	enable_irq(8);
}



// # int mp1_ioctl(unsigned long arg, unsigned int cmd)
// # The dispatch function for the MP1 ioctls - should use the cmd argument
// # and a jumptable to execute one of the specific ioctls implemented below.
// # Inputs   : unsigned long arg - parameter to the mp1_ioctl_....
// #          : unsigned int cmd  - specifies which mp1_ioctl_... function 
// #          :                     to execute
// # Outputs  : Returns an integer - depends on which ioctl() function is called
// # Registers: Standard C calling convention

// .globl mp1_ioctl

// 		jump_table: 			#declaring a jump table
// 		.long mp1_ioctl_startgame, mp1_ioctl_addmissile, mp1_ioctl_movexhairs, mp1_ioctl_getstatus, mp1_ioctl_endgame
// mp1_ioctl:

// 		movl 8(%esp), %ebx			#save argument to ebx 		

// 		jmp *jump_table(,%ebx,4)	#setting up switch case 	

// 		cmpl $0, %ebx 				#if less than 0
// 		jl finish
// 		cmpl $4,%ebx 				#if greater then 4
// 		jg finish
// 		jmp *jump_table(,%ebx,4) 	#go to the case 	

// 		finish:
// 		movl $-1, %eax 				#ret -1 if invalid
// 		ret




// system calls: we have a system call handler loaded at 80 and it includes a jump table(in assembly) and function in C

// filesystem: There is already a filesystem. FS is a read only file system. need data structs like inode, Dentry, etc. 
// Also, you would need to find the loaded file system in memory for you to do anything on it. 

// Figure out how you can use the fields in the boot block to move through the filesystem.



// System calls are invoked using int $0x80.
// The call number is placed in EAX, the first argument in EBX, then ECX, and finally EDX. 
// You should protect all of the registers from modification by the system call to avoid leaking information to the user programs.
// The return value is placed in EAX if the call returns (not all do); a value of -1 indicates an error, while others indicate some form of success. successful calls should return 0


int32_t ece391_open(const uint8_t* filename)
{

//The open system call provides access to the file system.
	//find the directory entry corresponding to the filename
	//allocate an unused file descriptor
	//set up any data necessary to handle the given type of file (directory, RTC device, or regular file)
	//If the named file does not exist or no descriptors are free, the call returns -1.

	// pushl %eax;					save all the registers to prevent information from leaking to the user programs
	// pushl %ebx;
	// pushl %ecx;
	// pushl %edx;
	// pushl %esi;
	// pushl %edi;
	// pushl %ebp;
	// pushl %esp;

	// movl filename, %ebx

	// if(filename does not exist or no descriptors are free)
	// {
	// 	movl $-1, %eax 				;return -1
	// }

	// else
	// {

	// }

	// popl %esp;					get back all the saved registers
	// popl %ebp;
	// popl %edi;
	// popl %esi;
	// popl %edx;
	// popl %ecx;
	// popl %ebx;
	// popl %eax;

	// movl $0, %eax 				;return 0
	return 0;	
}

int32_t ece391_read(int32_t fd, void* buf, int32_t nbytes)
{
	//For RTC, this call should always return 0, but only after an interrupt has occurred.
	//set a flag and wait until the interrupt handler clears it then return 0

	while(1)
	{
		if(interrupt_number > 0)
		break;	
	} 
	interrupt_number = 0;
	return 0;
}

int32_t ece391_write(int32_t fd, const void* buf, int32_t nbytes)
{
// The write system call writes data to the terminal or to a device (RTC). In the case of the terminal, all data should
// 	be displayed to the screen immediately. In the case of the RTC, the system call should always accept only a 4-byte
// 		integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts accordingly. Writes to regular
// 			files should always return -1 to indicate failure since the file system is read-only. The call returns the number of bytes
// 				written, or -1 on failure.

	return 0;
}

int32_t ece391_close(int32_t fd)
{
// The close system call closes the specified file descriptor and makes it available for return from later calls to open.
// 	You should not allow the user to close the default descriptors (0 for input and 1 for output). Trying to close an invalid
// 		descriptor should result in a return value of -1; successful closes should return 0.
	return 0;
}
//how the RTC works and what the device driver needs to do to communicate with it.
//Virtualizing the RTC is not required, but does make testing easier when you run multiple programs with the RTC open.
