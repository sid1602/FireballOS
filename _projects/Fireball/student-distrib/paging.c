/* paging.c
 * vim:ts=4 noexpandtab
 */

#include "paging.h"
#include "lib.h"

static PDE_t PD[NUM_PDE] __attribute__((aligned (0x400000)));
static PTE_t VIDEO_PT[NUM_PTE] __attribute__((aligned (0x1000)));

void init_paging()
{
	// register uint32_t *foo asm ("eax");
	// printf("Initializing paging\n");
	// printf("CRO : 0x%x\n", foo);


	init_PD();
	init_VIDEO_PT();


	//set_PSE();
	asm volatile("movl %%cr4, %%eax \n\t"		
		 		 "orl  0x00000010, %%eax \n\t"	
				 "movl %%eax, %%cr4"			
				 :								
				 :								
				 : "eax", "memory"				
				 );

	//set_paging();
		asm volatile("movl %%cr0, %%eax \n\t"		
		 		 "orl  0x80000000, %%eax \n\t"	
				 "movl %%eax, %%cr0"			
				 :								
				 :								
				 : "eax", "memory"				
				 );   

	// register uint32_t *bar asm ("eax");
	// printf("CR0[31] set to 1\n");
	// printf("CRO : 0x%x\n", bar);
}

void init_PD()
{
	PDE_t PDE_default;
	PDE_default.present = 0;
	PDE_default.read_write = 0;
	PDE_default.user_super = 0;
	PDE_default.write_through = 0;
	PDE_default.cache_disabled = 1;
	PDE_default.accessed = 0;
	PDE_default.reserved = 0;
	PDE_default.page_size = 0;
	PDE_default.global = 0;
	PDE_default.avail = 0;
	PDE_default.page_table_base = 0;

	int i;
	for(i = 0; i < NUM_PDE; i++)
	{
		PD[i] = PDE_default;
		//printf("%d", PD[i+1024].global);
	}

	// Set up video memory PDE
	int video_offset;
	video_offset = get_PDE_offset(VIDEO_MEM);

	
	// PD[video_offset].read_write = 1;
	// PD[video_offset].user_super = 1;
	PDE_t PDE_video;
	PDE_video = PDE_default;
	PDE_video.global = 1;
	SET_PDE_4KB_PT(PDE_video, VIDEO_PT);
	PD[video_offset] = PDE_video;

	// Set up kernel memory PDE
	int kernel_offset;
	kernel_offset = get_PDE_offset(KERNEL_MEM);

	PDE_t PDE_kernel;
	PDE_kernel = PDE_default;
	PDE_kernel.global = 1;
	PDE_kernel.user_super = 0;
	SET_PDE_4MB_PAGE(PDE_kernel, KERNEL_MEM);
	PD[kernel_offset] = PDE_kernel;

 	// set_PDBR(PD);
 	uint32_t CR3;
 	asm volatile("movl %1, %%eax \n\t"			
				 "movl %%eax, %%cr3\n\t"			
				 "movl %%cr3, %0"			
				 : "=r"	(CR3)							
				 : "r" (PD)				
				 : "eax", "memory"				
				 ); 
}

void init_VIDEO_PT()
{
	PTE_t PTE_default;
	PTE_default.present = 0;
	PTE_default.read_write = 0;
	PTE_default.user_super = 0;
	PTE_default.write_through = 0;
	PTE_default.cache_disabled = 1;
	PTE_default.accessed = 0;
	PTE_default.dirty = 0;
	PTE_default.page_table_attribute = 0;
	PTE_default.global = 0;
	PTE_default.avail = 0;
	PTE_default.page_base = 0;


	int i;
	for(i = 0; i < NUM_PTE; i++)
	{
		VIDEO_PT[i] = PTE_default;
		//if(i == 0xB8000)
		//	VIDEO_PT[i];
	}

	// Set up the video page table
	int offset = get_PTE_offset(VIDEO_MEM);

	PTE_t PTE_video;
	PTE_video = PTE_default;
	PTE_video.global = 1;
	SET_PTE(PTE_video, VIDEO_MEM);
	VIDEO_PT[offset] = PTE_video;
	//VIDEO_PT[offset].present = 1;
	//VIDEO_PT[0].present = 0;
}

uint32_t get_PDE_offset(uint32_t addr)
{
	return (addr & 0xFFC00000) >> 22;
}

uint32_t get_PTE_offset(uint32_t addr)
{
	return (addr & 0x003FF000) >> 12;
}

uint32_t get_Page_offset(uint32_t addr)
{
	return (addr & 0x00000FFF);
}

void test_paging()
{
	uint32_t *a = (uint32_t*) 0x000B8000;
	int i;
	for(i = 0; i < 100; i++, a += 400)
	{
		printf("\n%x\n", a[i]);
		
	}
}


