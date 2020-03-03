#ifndef _CORUN_H
#define _CORUN_h

#include <semaphore.h>
#include "cpu.h"
#include "process.h"
#include "argument.h"
#include "perf_counter.h"

#define DEFAULT_PERIOD 100000
#define MUTEX_MAP_FILE "/tmp/corun_lock.lck"

struct corun_param
{
	struct corun_arg *pargs;
	struct task_list *ptask;
	ctr_list_t **events;
	sem_t *mutex;
	unsigned int num_arg;
};

int corun_param_init(struct corun_param *param, int argc, char *argv[]);
void corun_param_free(struct corun_param *param);
int task_coruning(struct corun_param *param);
void collect_data(struct corun_param *param);

#endif
