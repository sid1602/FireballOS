/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
//////////////////	WRITTEN BY ME 	/////////////////////////////////////////////////////////////////////////
#include "rtc.h"
#include "int_handler.h"


#define PIC1			0x20		/*IO base address for master PIC*/
#define PIC2			0xA0		/*IO base address for slave PIC */
#define PIC1_COMMAND	PIC1 		
#define PIC1_DATA		(PIC1+1) 	/*This is basically port 0x21	*/
#define PIC2_COMMAND	PIC2
#define PIC2_DATA		(PIC2+1) 	/*This is basically port 0xA1	*/
#define PIC_EOI			0x20 		/*End-of-interrupt command code	*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
entry (unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;

	/* Clear the screen. */
	clear();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%#x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	printf ("flags = 0x%#x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf ("boot_device = 0x%#x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf ("cmdline = %s\n", (char *) mbi->cmdline);

	if (CHECK_FLAG (mbi->flags, 3)) {
		int mod_count = 0;
		int i;
		module_t* mod = (module_t*)mbi->mods_addr;
		while(mod_count < mbi->mods_count) {
			printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
			printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
			printf("First few bytes of module:\n");
			for(i = 0; i<16; i++) {
				printf("0x%x ", *((char*)(mod->mod_start+i)));
			}
			printf("\n");
			mod_count++;
			mod++;
		}
	}
	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf ("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		elf_section_header_table_t *elf_sec = &(mbi->elf_sec);

		printf ("elf_sec: num = %u, size = 0x%#x,"
				" addr = 0x%#x, shndx = 0x%#x\n",
				(unsigned) elf_sec->num, (unsigned) elf_sec->size,
				(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		memory_map_t *mmap;

		printf ("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap
					+ mmap->size + sizeof (mmap->size)))
			printf (" size = 0x%x,     base_addr = 0x%#x%#x\n"
					"     type = 0x%x,  length    = 0x%#x%#x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->type,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low);
	}

	/* Construct an LDT entry in the GDT */
	{
		seg_desc_t the_ldt_desc;
		the_ldt_desc.granularity    = 0;
		the_ldt_desc.opsize         = 1;
		the_ldt_desc.reserved       = 0;
		the_ldt_desc.avail          = 0;
		the_ldt_desc.present        = 1;
		the_ldt_desc.dpl            = 0x0;
		the_ldt_desc.sys            = 0;
		the_ldt_desc.type           = 0x2;

		SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
		ldt_desc_ptr = the_ldt_desc;
		lldt(KERNEL_LDT);
	}

	/* Construct a TSS entry in the GDT */
	{
		seg_desc_t the_tss_desc;
		the_tss_desc.granularity    = 0;
		the_tss_desc.opsize         = 0;
		the_tss_desc.reserved       = 0;
		the_tss_desc.avail          = 0;
		the_tss_desc.seg_lim_19_16  = TSS_SIZE & 0x000F0000;
		the_tss_desc.present        = 1;
		the_tss_desc.dpl            = 0x0;
		the_tss_desc.sys            = 0;
		the_tss_desc.type           = 0x9;
		the_tss_desc.seg_lim_15_00  = TSS_SIZE & 0x0000FFFF;

		SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

		tss_desc_ptr = the_tss_desc;

		tss.ldt_segment_selector = KERNEL_LDT;
		tss.ss0 = KERNEL_DS;
		tss.esp0 = 0x800000;
		ltr(KERNEL_TSS);
	}
	/* DUMMY INTERRUPT DESCRIPTOR TABLE ENTRY */
	{	
		int i = 0;
		idt_desc_t idt_entry;
		for(i=0; i<32; i++)
		{
			SET_IDT_ENTRY(idt[i], dummy_int_handler);
			idt_entry.seg_selector = KERNEL_CS;
			idt_entry.dpl = 0;
		}
	}

	/* KEYBOARD INTERRUPT DESCRIPTOR TABLE ENTRY */
	{
		idt_desc_t idt_entry;
		SET_IDT_ENTRY(idt[33], kbd_int_handler);
		idt_entry.seg_selector = KERNEL_CS;
		idt_entry.dpl = 0;
	}

	/* RTC INTERRUPT DESCRIPTOR TABLE ENTRY */
	{
		idt_desc_t idt_entry;
		SET_IDT_ENTRY(idt[40], rtc_int_handler);
		idt_entry.seg_selector = KERNEL_CS;
		idt_entry.dpl = 0;
	}

	printf("SJDGSDJKGHSLDKGHSDLGKJSHDLGKJSHDGLKSJHGDLSDKJGHSDG");
	/* Init the PIC */
	i8259_init();

	/* Initialize devices, memory, filesystem, enable device interrupts on the
	 * PIC, any other initialization stuff... */


//////////////////////	WRITTEN BY ME 	/////////////////////////////////////////////////////////////////////////////
//RTC interrupts are disabled by default. If you turn on the RTC interrupts, the RTC will periodically generate IRQ 8.
//When programming the RTC, it is important that the NMI (non-maskable-interrupt) and other interrupts are disabled.
//The 2 IO ports used for the RTC and CMOS are 0x70 and 0x71.
//Port 0x70 is used to specify an index or "register number", and to disable NMI. 
//Port 0x71 is used to read or write from/to that byte of CMOS configuration space.
//Only three bytes of CMOS RAM are used to control the RTC periodic interrupt function. 
//They are called RTC Status Register A, B, and C. They are at offset 0xA, 0xB, and 0xC in the CMOS RAM. 

	//disable_ints();		// important that no interrupts happen (perform a CLI)

	init_rtc();				//
	init_keyboard();		//
	
//	enable_ints();		// (perform an STI) and reenable NMI if you wish
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	/* Enable interrupts */
	/* Do not enable the following until after you have set up your
	 * IDT correctly otherwise QEMU will triple fault and simple close
	 * without showing you any output */
	/*printf("Enabling Interrupts\n");
	sti();*/

	/* Execute the first program (`shell') ... */

	/* Spin (nicely, so we don't chew up cycles) */
	asm volatile(".1: hlt; jmp .1;");
}
