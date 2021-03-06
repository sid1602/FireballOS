/************************************************/
/*Created by Siddharth Murali, Aabhas Sharma,	*/	
/*			 James Lang, Shubham Agarwal		*/
/*	9th Dec, 2014								*/
/*												*/
/************************************************/

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "keyboard.h"
#include "buffer.h"
#include "lib.h"
#include "systemcalls.h"

#define NUM_COLS 80
#define TERM_ROWS 25

typedef struct buff_attr
{
 	//display attributes
 	int curr_line;
 	int curr_limit;
 	int curr_x;
 	int curr_y;
}buff_attr;

//extern node buffer[NUM_COLS*NUM_ROWS];
extern node* screens[3];
extern int screen_num;
extern char* status;
int process_buf;

//base functions (3.1 - 3.4)
extern void terminal_init();
extern node* pass_buff();

//required functions
extern node* terminal_open(file_t* file, const uint8_t* filename);
extern int32_t terminal_read(file_t* file, uint8_t* buf, int32_t counter);
extern int32_t terminal_write(file_t* file, const uint8_t* buf, int32_t counter);
extern int32_t terminal_close(file_t* file);

//3.5 helpers
extern void terminal_switch(int new_screen, int old_screen);
int32_t screen_assign(int index);

//extra credit
extern void status_bar();
void switch_status();
char* embed_time(char* status);
void get_system_time();

extern driver_jt_t stdin_jt;
extern driver_jt_t stdout_jt;
extern char* status;

#endif /* TERMINAL */
