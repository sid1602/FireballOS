/* paging.c
 * vim:ts=4 noexpandtab
 */

#include "paging.h"
#include "lib.h"

// static PDE_t PD[NUM_PDE] __attribute__((aligned (0x1000)));
// static PDE_t PD0[NUM_PDE] __attribute__((aligned (0x1000)));
// static PDE_t PD1[NUM_PDE] __attribute__((aligned (0x1000)));

static PDE_t PD[NUM_PD][NUM_PDE] __attribute__((aligned (0x1000)));
static PTE_t PT[NUM_PT][NUM_PTE] __attribute__((aligned (0x1000)));

static uint32_t PGRM_PAGE[7];

// static PTE_t VIDEO_PT[NUM_PTE] __attribute__((aligned (0x1000)));

static PDE_t PDE_default;
static PTE_t PTE_default;

static PDE_t* cur_PD = NULL;

/*
 * void init_paging()
 * 	 Description: Initializes page directories, tables and control registers
 *   Inputs: none
 *   Return Value: none
 */
void init_paging()
{
	// Set up program page (physical) address array
	uint32_t i;
	for(i = 0; i < NUM_PD; i++)
	{
		PGRM_PAGE[i] = PAGE1 + i * PAGE_SIZE;
	}

	// Initialize the Page directory
	init_PD();

	// Initialize the video page table
	init_VIDEO_PT();

	// Set PSE bit in CR4
	set_PSE();

	// Set PG bit in CR0
	set_PG();

	// Set PGE bit in CR4
	set_PGE();

	//test_paging();

}

/*
 * void init_PD()
 * 	 Description: Sets all entries in the page directory
 *   Inputs: none
 *   Return Value: none
 */
void init_PD()
{
	// Fill the page directory with blank entries
	PDE_default.present = 0;
	PDE_default.read_write = 0;
	PDE_default.user_super = 0;
	PDE_default.write_through = 0;
	PDE_default.cache_disabled = 0;
	PDE_default.accessed = 0;
	PDE_default.reserved = 0;
	PDE_default.page_size = 0;
	PDE_default.global = 0;
	PDE_default.avail = 0;
	PDE_default.page_table_base = 0;

	int i;
	for(i = 0; i < NUM_PDE; i++)
	{
		PD[0][i] = PDE_default;
	}

 	cur_PD = PD[0];

	// Set up video memory PDE
	int video_offset;
	video_offset = get_PDE_offset(VIDEO_MEM);

	PDE_t PDE_video;
	PDE_video = PDE_default;
	//SET_PDE_4KB_PT(PDE_video, VIDEO_PT);
	SET_PDE_4KB_PT(PDE_video, PT[0]);
	cur_PD[video_offset] = PDE_video;

	// Set up kernel memory PDE
	int kernel_offset;
	kernel_offset = get_PDE_offset(KERNEL_MEM);

	PDE_t PDE_kernel;
	PDE_kernel = PDE_default;
	PDE_kernel.global = 1;
	SET_PDE_4MB_PAGE(PDE_kernel, KERNEL_MEM);
	cur_PD[kernel_offset] = PDE_kernel;

	// Pass the address of the page directory into the PBDR
 	set_PDBR(cur_PD);

 	// Temporarily copy all of PD[0] into all other PDs
 	int j, k;
 	for(j = 1; j < NUM_PD; j++)
 	{
		for(k = 0; k < NUM_PDE; k++)
		{
			PD[j][k] = PD[0][k];
		}
	}

}

/*
 * void init_VIDEO_PT()
 * 	 Description: Sets up video memory mapping for kernel
 *   Inputs: none
 *   Return Value: none
 */
void init_VIDEO_PT()
{
	// Fill the video page table with blank entries
	PTE_default.present = 0;
	PTE_default.read_write = 0;
	PTE_default.user_super = 0;
	PTE_default.write_through = 0;
	PTE_default.cache_disabled = 0;
	PTE_default.accessed = 0;
	PTE_default.dirty = 0;
	PTE_default.page_table_attribute = 0;
	PTE_default.global = 0;
	PTE_default.avail = 0;
	PTE_default.page_base = 0;

	int i, j;
	for(i = 0; i < NUM_PT; i++)
	{
		for(j = 0; j < NUM_PTE; j++)
		{
			PT[i][j] = PTE_default;
		}
	}

	// Set up the kernel video PTE
	int offset = get_PTE_offset(VIDEO_MEM);

	PTE_t PTE_video;
	PTE_video = PTE_default;
	PTE_video.global = 1;
	SET_PTE(PTE_video, VIDEO_MEM);
	PT[0][offset] = PTE_video;
}

/*
 * uint32_t get_PDE_offset(uint32_t addr)
 * 	 Description: Finds the PDE which a virtual address belongs to
 *   Inputs: addr - A virtual address
 *   Return Value: The index of the PDE for this address
 */
uint32_t get_PDE_offset(uint32_t addr)
{
	return (addr & 0xFFC00000) >> 22;
}

/*
 * uint32_t get_PTE_offset(uint32_t addr)
 * 	 Description: Finds the PTE which a virtual address belongs to
 *   Inputs: addr - A virtual address
 *   Return Value: The index of the PTE for this address
 */
uint32_t get_PTE_offset(uint32_t addr)
{
	return (addr & 0x003FF000) >> 12;
}

/*
 * uint32_t get_Page_offset(uint32_t addr)
 * 	 Description: Finds the offset of an address within its 4KB page
 *   Inputs: addr - A virtual address
 *   Return Value: The offset of the address
 */
uint32_t get_Page_offset(uint32_t addr)
{
	return (addr & 0x00000FFF);
}

/*
 * uint32_t get_Page_offset(uint32_t addr)
 * 	 Description: Finds the offset of an address within its 4KB page
 *   Inputs: addr - A virtual address
 *   Return Value: The offset of the address
 */
// Returns parent process PD or -1 on fail
PDE_t* task_mem_init(uint32_t PID)
{
	if(PID < 0 || PID > 6)
		return NULL;

	cur_PD = PD[PID];
	map_4MB_page(PGRM_IMG, PGRM_PAGE[PID], 1, 1);
	set_PDBR(cur_PD);

	return cur_PD;
}

uint8_t* user_vidmap()
{
	// Set up PTE if necessary
	uint32_t PT_index = get_PTE_offset(USER_VID);

	if(PT[1][PT_index].present == 0)
	{
		PTE_t PTE_video;
		PTE_video = PTE_default;
		PTE_video.user_super = 1;
		PTE_video.read_write = 1;

		SET_PTE(PTE_video, VIDEO_MEM);
		PT[1][PT_index] = PTE_video;
	}

	// Set up PDE if necessary
	uint32_t PD_index = get_PDE_offset(USER_VID);

	if(cur_PD[PD_index].present == 0)
	{
		PDE_t PDE_video;
		PDE_video = PDE_default;
		PDE_video.user_super = 1;
		PDE_video.read_write = 1;

		SET_PDE_4KB_PT(PDE_video, PT[1]);
		cur_PD[PD_index] = PDE_video;
	}

	INVLPG(USER_VID);

	return (uint8_t*)USER_VID;
}

void map_4MB_page(uint32_t vir_addr, uint32_t phys_addr, 
				  uint32_t user_super, uint32_t read_write)
{
	PDE_t PDE = PDE_default;

	uint32_t PD_offset;
	PD_offset = get_PDE_offset(vir_addr);

	PDE.user_super = user_super; 
	PDE.read_write = read_write;
	SET_PDE_4MB_PAGE(PDE, phys_addr);

	cur_PD[PD_offset] = PDE;
	//INVLPG(vir_addr);
}


/*
 * void test_paging()
 * 	Description: Attempts to access specific places in memory
 *   Inputs: none
 *   Return Value: none
 */
void test_paging()
{
	// Access invalid momory
	// uint8_t* a = (uint8_t*) 0x00000000;
	// printf("\n%x\n", *a);

	// Access Kernel memory
	uint8_t* b = (uint8_t*) 0x00400000;
	printf("\n%x\n", *b);

	// Access Video memory
	// uint8_t* c = (uint8_t*) 0x000B8000;
	// printf("\n%x\n", *c);
}

PDE_t* get_curr_PD()
{
	return cur_PD;
}
