#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "perf_counter.h"

void delay(uint64_t time)
{
	while(time--);
}

int main()
{
	ctr_list_t *events_list;
	const char *events_name[] = {
	"PERF_COUNT_HW_INSTRUCTIONS",
	"PERF_COUNT_HW_CPU_CYCLES",
	"PERF_COUNT_HW_CACHE_MISSES",
	"LLC_MISSES"
	};
	int events_num = sizeof(events_name) / sizeof(char*), sample_period = 10000, i;
	uint64_t *results = (uint64_t*)malloc(events_num * sizeof(uint64_t));

	event_counter_initialize();
	init_events_list(&events_list, events_name, events_num, sample_period);
	attach_ctrs(events_list, 0);
	reset_all_event_counter(events_list);

	enable_all_event_counter(events_list);
	printf("test for this printf\n");
	disable_all_event_counter(events_list);

	read_counter(results, events_list);
	for(i = 0; i < events_num; i++)
	{
		printf("%lu\n", results[i]);
	}

	event_counter_finitial(events_list);
	return 0;
}
