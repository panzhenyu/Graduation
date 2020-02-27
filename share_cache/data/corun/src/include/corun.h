#ifndef _CORUN_H
#define _CORUN_h

#include "process.h"
#include "argument.h"
#include "perf_counter.h"

#define DEFAULT_PERIOD 100000

struct task_list* args2tasks(struct corun_arg *args, unsigned int num_arg);
unsigned long args2period(struct corun_arg *args, unsigned int num_arg);
ctr_list_t* args2events(struct corun_arg *args, unsigned int num_arg);
void create_process(struct task_list *ptask);

#endif
