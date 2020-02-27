#include <stdio.h>
#include <stdlib.h>
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

void create_tasks(struct task_list *ptask)
{
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
	create_tasks(tasks);

	// free all resources
FREE:
	event_counter_finitial(events);
	task_list_free(tasks);
	free_all_args(args, num_arg);
}

