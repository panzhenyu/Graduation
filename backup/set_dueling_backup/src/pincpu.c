#include <unistd.h>
#define __USE_GNU
#include <sched.h>
#include "pincpu.h"

int pin_cpu(int cpu)
{
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(cpu, &mask);
	if(sched_setaffinity(0, sizeof(mask), &mask) < 0)
		return 0;
	return 1;
}
