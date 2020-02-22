#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <perfmon/pfmlib.h>
#include <perfmon/perf_event.h>
#include <perfmon/pfmlib_perf_event.h>

int main()
{
	if(pfm_initialize() != PFMLIB_SUCCESS)
	{
		printf("cannot initialize libpfm\n");
		exit(1);
	}
	pfmlib_input_param_t inp;
	memset(&inp, 0, sizeof(inp));
	int ret = pfm_find_event("IA64_TAGGED_INST_RETIRED_IBRP0_PMC8", &inp.pfp_events[0].event);
	if(ret != PFMLIB_SUCCESS)
	{
		printf("ia64_tagged_inst_retired not found\n");
		exit(1);
	}
	return 0;
}
