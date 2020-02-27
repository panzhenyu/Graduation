#include <perfmon/pfmlib_perf_event.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	if(pfm_initialize())
	{
		printf("pfm_initialize failed!\n");
		exit(1);
	}
	struct perf_event_attr attr;
	memset(&attr, 0, sizeof(struct perf_event_attr));
	int ret = pfm_get_perf_event_encoding(argv[1], PFM_PLM3, &attr, NULL, NULL);
	if(ret != PFM_SUCCESS)
		printf("Failed to get event id\n");
	else
		printf("id = %lu\n", attr.config);
	pfm_terminate();
	return 0;
}
