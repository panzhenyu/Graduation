#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "include/corun.h"

static struct task_list* args2tasks(struct corun_arg *args, unsigned int num_arg)
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

static unsigned long args2period(struct corun_arg *args, unsigned int num_arg)
{
	int i;
	for(i = 0; i < num_arg; i++)
	{
		if(args[i].type == PERIOD)
			return atol(args[i].param);
	}
	return DEFAULT_PERIOD;
}

static ctr_list_t* args2events(struct corun_arg *args, unsigned int num_arg)
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

static void mutex_init(sem_t *ptr)
{
	int fd = open(MUTEX_MAP_FILE, O_RDWR|O_CREAT, S_IRWXU);
	if(fd < 0)
		goto FAILED;
	write(fd, "init", 4);
	ptr = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if(ptr == MAP_FAILED)
		goto FAILED;
FAILED:
	ptr = NULL;
}

static int create_process(struct task_list *ptask, void *run_arg)
{
	if(!ptask)
		return 0;
	int task_num = ptask->num, i, cpu_idx;
	struct task_desc *task = ptask->task;
	for(i = 0; i < task_num; i++)
	{
		cpu_idx = malloc_cpu();
		if(cpu_idx < 0)
		{
			printf("too many processes\n");
			return 0;
		}
		task[i].cpu = cpu_idx;
		task[i].pid = fork();
		if(!task[i].pid)
			run_task(&task[i], run_arg);
	}
	return 1;
}

// must enable perf event counter first
int corun_param_init(struct corun_param *param, int argc, char *argv[])
{
	if(param == NULL || argc == 0 || argv == NULL)
		return 0;
	int num_task, i;
	param->pargs = NULL;
	param->ptask = NULL;
	param->events = NULL;
	param->mutex = NULL;

	param->pargs = get_all_arguments(argc, argv, &param->num_arg);

	param->ptask = args2tasks(param->pargs, param->num_arg);
	num_task = param->ptask->num;

	param->events = (ctr_list_t**)malloc(sizeof(ctr_list_t*) * num_task);
	memset(param->events, 0, sizeof(ctr_list_t*) * num_task);
	for(i = 0; i < num_task; i++)
		param->events[i] = args2events(param->pargs, param->num_arg);

	mutex_init(param->mutex);
	if(!param->ptask || !param->events[0] || !param->mutex)
		return 0;
	return 1;
}

void corun_param_free(struct corun_param *param)
{
	if(param == NULL)
		return;
	int task_num, i;
	if(param->ptask)
	{
		task_num = param->ptask->num;
		task_list_free(param->ptask);
	}
	if(param->events)
	{
		for(i = 0; i < task_num; i++)
			free_event_list(param->events[i]);
		free(param->events);
	}
	if(param->pargs)
		free_all_args(param->pargs, param->num_arg);
}

int task_coruning(struct corun_param *param)
{
	if(param == NULL)
		return 0;

	int i, num_task;
	struct task_list *task_list = param->ptask;
	ctr_list_t **events = param->events;
	sem_t *mutex = param->mutex;
	num_task = task_list->num;

	if(create_process(task_list, mutex) == 0)
		return 0;
	for(i = 0; i < num_task; i++)
	{
		if(attach_ctrs(events[i], task_list->task[i].pid) == -1)
		{
			printf("attach ctrs failed\n");
			return 0;
		}
		reset_all_event_counter(events[i]);
	}
	for(i = 0; i < num_task; i++)
	{
		sem_post(mutex);
		enable_all_event_counter(events[i]);
	}
	waitpid(-1, NULL, 0);
	for(i = 0; i < num_task; i++)
		disable_all_event_counter(events[i]);
	printf("all child process have finished!\n");
	return 1;
}

void collect_data(struct corun_param *param)
{
	if(param == NULL)
		return;
	uint64_t *data;
	int i, j, event_num, task_num;

	ctr_list_t **events = param->events;
	event_num = ctrs_len(events[0]);
	task_num = param->ptask->num;

	data = (uint64_t*)malloc(sizeof(uint64_t) * event_num);
	memset(data, 0, sizeof(uint64_t) * event_num);

	printf("Results: ");
	for(j = 0; j < task_num; j++)
	{
		read_counter(data, events[j]);
		for(i = 0; i < event_num-1; i++)
			printf("%lu ", data[i]);
		printf("%lu\n", data[i]);
	}
	free(data);
}

int main(int argc, char *argv[])
{
	event_counter_initialize();

	struct corun_param *param = (struct corun_param*)malloc(sizeof(struct corun_param));

	// initialize all parameters
	corun_param_init(param, argc, argv);

	// free all resources
	corun_param_free(param);
	free(param);
	event_counter_finitial();

}

