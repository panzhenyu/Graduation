#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "include/process.h"

int set_fifo()
{
	pid_t pid = getpid();
	struct sched_param param;
	param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	if(sched_setscheduler(pid, SCHED_FIFO, &param) < 0)
		return 0;
	return 1;
}

struct task_list* task_list_init(char *command[], int num)
{
	if(!command || num == 0)
		return NULL;
	int i;
	struct task_list *ptask;
	struct task_desc *t;
	ptask = (struct task_list*)malloc(sizeof(struct task_list));
	if(!ptask)
		return NULL;
	ptask->task = (struct task_desc*)malloc(sizeof(struct task_desc) * num);
	if(!ptask->task)
		return NULL;
	ptask->num = num;

	t = ptask->task;
	for(i = 0; i < ptask->num; i++)
	{
		t[i].cpu = core_num() + 1;
		t[i].pid = t[i].status = -1;
		t[i].cmd = strdup(command[i]);
	}
	return ptask;
}

void task_list_free(struct task_list *ptask)
{
	if(ptask)
	{
		struct task_desc *td = ptask->task;
		int num = ptask->num, i;
		for(i = 0; i < num; i++)
		{
			if(td[i].cmd)
				free(td[i].cmd);
			if(td[i].pid > 0)
				kill(td[i].pid, SIGKILL);
		}
		free(td);
		free(ptask);
	}
}

void task_desc_show(struct task_desc *desc)
{
	printf("status: %-6dpid: %-6dcpu: %-6ucmd: %s\n",
		desc->status, desc->pid, desc->cpu, desc->cmd);
}

void task_list_show(struct task_list *ptask)
{
	if(!ptask)
		return;
	unsigned int i;
	struct task_desc *task = ptask->task;
	puts("--------TASKS--------");
	for(i = 0; i < ptask->num; i++)
		task_desc_show(&task[i]);
	puts("---------------------");
	printf("\n");
}
