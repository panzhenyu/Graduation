#ifndef _PROCESS_H
#define _PROCESS_H

#include <unistd.h>

struct process_list
{
	pid_t *process;
	unsigned int num_proc;
};

int set_fifo();
struct process_list *process_list_init(char *command[]);

#endif
