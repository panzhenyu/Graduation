#ifndef _PROCESS_H
#define _PROCESS_H

#include <unistd.h>
#include "cpu.h"

struct task_desc
{
	int status;
	char *cmd;
	pid_t pid;
	cpu_t cpu;
};

struct task_list
{
	struct task_desc *task;
	unsigned int num;
};

int set_fifo();
struct task_list* task_list_init(char *command[], int num);
void task_list_free(struct task_list *ptask);
void task_list_show(struct task_list *ptask);
void task_desc_show(struct task_desc *desc);

#endif
