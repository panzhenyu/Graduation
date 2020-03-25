#include <wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "perf_counter.h"

#define DEFAULT_SAMPLE_PERIOD 10000

int main()
{
	ctr_list_t *parent_events, *child_events;
	pid_t child_pid;
	const char* events_name[] = {
	"PERF_COUNT_HW_CPU_CYCLES",
	"PERF_COUNT_HW_CACHE_MISSES"
	};
	int len = sizeof(events_name) / sizeof(char*);
	printf("%d\n", len);
	event_counter_initialize();
	init_events_list(&parent_events, events_name, len, DEFAULT_SAMPLE_PERIOD);
	init_events_list(&child_events, events_name, len, DEFAULT_SAMPLE_PERIOD);
	attach_ctrs(parent_events, 0);
	child_pid = fork();
	if(child_pid < 0)
	{
		printf("fork error!\n");
		exit(1);
	}
	else if(child_pid == 0)
	{
		const char* filename = "./random_access";
		char* const argv[] = {"./random_access", NULL};
		char* const envp[] = {NULL};
		sleep(10);
		if(execve(filename, argv, envp) < 0)
		{
			printf("failed to execute child process\n");
			exit(2);
		}
	}
	else
	{
		printf("here\n");
		attach_ctrs(child_events, child_pid);
		reset_all_event_counter(child_events);
		enable_all_event_counter(child_events);
		wait(0);
		disable_all_event_counter(child_events);
		uint64_t *results = (uint64_t*)malloc(len * sizeof(uint64_t));
		read_counter(results, child_events);
		int i;
		printf("result--->");
		for(i = 0; i < len; i++)
			printf("%lu ", results[i]);
		printf("\n");
		event_counter_finitial(child_events);
	}
}
