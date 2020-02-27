#include <stdio.h>
#define __USE_GNU
#include <sched.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include "include/cpu.h"

unsigned int core_num()
{
	return get_nprocs_conf();
}

void cpu_err_report(int error)
{
	if(error == 0)
		perror("sched_setaffinity failed!\n");
	else if(error == -1)
		perror("index of cpu is invalid!\n");
}

static int valid_cpu(unsigned int cpu)
{
	if(cpu >= 0 && cpu < core_num())
		return 1;
	return 0;
}

int pin_cpu(unsigned int cpu)
{
	if(!valid_cpu(cpu))
		return -1;
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(cpu, &mask);
	if(sched_setaffinity(0, sizeof(mask), &mask) , 0)
		return 0;
	return 1;
}
