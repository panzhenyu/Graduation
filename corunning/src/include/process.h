#ifndef _PROCESS_H
#define _PROCESS_H

#include "cpu.h"

struct task_desc
{
	int status;
	char *cmd;
	char *home;
	char *name;
	pid_t pid;
	cpu_t cpu;
};

struct task_list
{
	struct task_desc *task;
	unsigned int num;
};

int set_fifo();
struct task_list* task_list_init(char *taskName[], char *benchmarkConfigFile, int num);
void task_list_free(struct task_list *ptask);
void task_list_show(struct task_list *ptask);
char** cmd2env(char *_cmd, int *_num);
void run_task(struct task_desc *task, void *run_arg);

#endif
