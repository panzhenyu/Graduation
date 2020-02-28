#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "include/cpu.h"
#include "include/corun.h"
#include "include/process.h"
#include "include/argument.h"
#include "include/perf_counter.h"

struct task_list* args2tasks(struct corun_arg *args, unsigned int num_arg)
{
	int i, j;
	char **command = (char**)malloc(sizeof(char*) * num_arg);
	for(i = 0, j = 0; i < num_arg; i++)
	{
		if(args[i].type == TASK)
			command[j++] = strdup(args[i].param);
	}

	struct task_list *tasks = task_list_init(command, j);

	for(i = 0; i < j; i++)
		free(command[i]);
	free(command);
	return tasks;
}

unsigned long args2period(struct corun_arg *args, unsigned int num_arg)
{
	int i;
	for(i = 0; i < num_arg; i++)
	{
		if(args[i].type == PERIOD)
			return atol(args[i].param);
	}
	return DEFAULT_PERIOD;
}

ctr_list_t* args2events(struct corun_arg *args, unsigned int num_arg)
{
	int i, j;
	char **events_name = (char**)malloc(sizeof(char*) * num_arg);
	for(i = 0, j = 0; i < num_arg; i++)
	{
		if(args[i].type == EVENT)
			events_name[j++] = strdup(args[i].param);
	}
	ctr_list_t *events = NULL;
	init_events_list(&events, events_name, j, (uint64_t)args2period);

	for(i = 0; i < j; i++)
		free(events_name[i]);
	free(events_name);
	return events;
}

void create_process(struct task_list *ptask)
{
	if(!ptask)
		return;
	int task_num = ptask->num, i, cpu_idx;
	struct task_desc *task = ptask->task;
	for(i = 0; i < task_num; i++)
	{
		cpu_idx = malloc_cpu();
		if(cpu_idx < 0)
		{
			printf("too many processes\n");
			break;
		}
		task[i].cpu = cpu_idx;
		task[i].pid = fork();
		if(!task[i].pid)
			run_task(&task[i]);
	}
}

int main(int argc, char *argv[])
{
	unsigned int num_arg;
	struct corun_arg *args = NULL;
	struct task_list *tasks = NULL;
	ctr_list_t *events = NULL;

	// initialize all parameters
	args = get_all_arguments(argc, argv, &num_arg);
	tasks = args2tasks(args, num_arg);
	event_counter_initialize();
	events = args2events(args, num_arg);

	// show all parameters
	printf("core_num: %u\n", core_num());
	printf("PERIOD : %lu\n", args2period(args, num_arg));
	show_all_arguments(args, num_arg);
	task_list_show(tasks);
	events_list_show(events);

	// check for parameters
	if(!tasks)
	{
		printf("tasks init failed\n");
		goto FREE;
	}
	if(!events)
	{
		printf("events init failed\n");
		goto FREE;
	}

	// run
	if(attach_ctrs(events, 0) == -1)
	{
		printf("attach ctrs failed\n");
		goto FREE;
	}
	reset_all_event_counter(events);
	enable_all_event_counter(events);

//	signal(SIGCHLD, SIG_IGN);
	create_process(tasks);
	waitpid(-1, NULL, 0);
//	wait(NULL);
	disable_all_event_counter(events);
	printf("all child process have finished!\n");

	// collect data
	int i, event_num;
	event_num = ctrs_len(events);
	uint64_t *data = (uint64_t*)malloc(sizeof(uint64_t) * event_num);
	memset(data, 0, sizeof(uint64_t) * event_num);
	read_counter(data, events);
	printf("Results: ");
	for(i = 0; i < event_num-1; i++)
		printf("%lu ", data[i]);
	printf("%lu\n", data[i]);
	free(data);
	// free all resources
FREE:
	event_counter_finitial(events);
	task_list_free(tasks);
	free_all_args(args, num_arg);
}

