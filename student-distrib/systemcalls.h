#ifndef _SYSTEMCALLS_H
#define _SYSTEMCALLS_H

#include "types.h"


//int32_t halt(uint8_t status);
int32_t execute(const uint8_t* command);
//int32_t read(int32_t fd, void* buf, int32_t nbytes);
//int32_t write(int32_t fd, const void* buf, int32_t nbytes);
int32_t open(const uint8_t* filename);
int32_t close(int32_t fd);
//int32_t getargs(uint8_t* buf, int32_t nbytes);
//int32_t vidmap(uint8_t** screen_start);
//int32_t set_handler(int32_t signum, void* handler_address);
//int32_t sigreturn(void);

/*	Helper functions for execute(const uint8_t* command)	*/
char* parse(char* input);
void get_arg(int i, char* input);
int32_t empty(void);
/***********************************************************************/
/*	PROCESS CONTROL BLOCK	*/
/***********************************************************************/
//each cell of PCB
typedef struct file_array{
	uint32_t* file_op;		 			//pointer to file operations table pointer
	uint32_t file_inode;				//inode pointer to inode number of file in file system
	uint32_t file_pos;					//keeps position of where we are in the file
	uint32_t flags;						//this flag tells us which fds are available
}file_array_t;

//PCB structure 
typedef struct pcb{
	file_array_t file_fds[8];			//array of open files are represented with a file array defined above
	uint32_t fd;						//integer index into this array is called a file descriptor and this integer is how user-level programs identify the open file
	uint32_t parent_bp;					//keep track of parent process' base pointer
	uint32_t parent_sp;					//keep track of parent prcoess' stack pointer
	uint8_t process_id;
//	uint32_t 
//	uint32_t
}pcb_t;

#endif /* _SYSTEMCALLS_H */