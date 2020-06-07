#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "include/corun.h"

static int countArgNum(char **arg)
{
	char **p = arg;
	int num = 0;
	while(*p)
	{
		num++;
		p++;
	}
	return num;
}

static void freeArg(char **arg)
{
	char **p = arg;
	while(*p)
	{
		free(*p);
		p++;
	}
	free(arg);
}

static ctr_list_t* args2events(char **eventNames, unsigned int period)
{
	if(!eventNames)
		return NULL;
	ctr_list_t *events = NULL;
	init_events_list(&events, eventNames, countArgNum(eventNames), period);
	return events;
}

static void mutex_init(sem_t **ptr)
{
	int fd = open(MUTEX_MAP_FILE, O_RDWR|O_CREAT, S_IRWXU);
	if(fd < 0)
	{
		printf("cannot open file: %s\n", MUTEX_MAP_FILE);
		*ptr = NULL;
		return;
	}
	write(fd, "-->init", 4);
	*ptr = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if(*ptr == MAP_FAILED)
	{
		printf("-->mmap failed\n");
		*ptr = NULL;
		return;
	}
	if(sem_init(*ptr, 1, 0) < 0)
	{
		printf("-->sem_init failed\n");
		munmap(*ptr, sizeof(sem_t));
		*ptr = NULL;
		return;
	}
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
			printf("-->too many processes\n");
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

	param->pargs = NULL;
	param->ptask = NULL;
	param->output = NULL;
	param->benchmarkConfigFile = NULL;
	param->events = NULL;
	param->mutex = NULL;
	param->period = DEFAULT_PERIOD;

	param->pargs = corun_arg_list_init(argc, argv);
	if(!param->pargs)
	{
		puts("-->corun_param_init: corun_arg_list_init failed");
		return 0;
	}

	char **taskNames, **eventNames, **period, **output, **benchmarkConfigFile;
	taskNames = corun_arg_list_getByOption(param->pargs, TASK);
	eventNames = corun_arg_list_getByOption(param->pargs, EVENT);
	period = corun_arg_list_getByOption(param->pargs, SAMPLE_PERIOD);
	output = corun_arg_list_getByOption(param->pargs, OUTPUT);
	benchmarkConfigFile = corun_arg_list_getByOption(param->pargs, CONFIG);
	if(!taskNames || !eventNames || !output || !benchmarkConfigFile)
	{
		puts("-->invalid arguments, needs task, event, output, benchmarkConfigFile");
		return 0;
	}

	param->ptask = task_list_init(taskNames, benchmarkConfigFile[0], countArgNum(taskNames));
	param->output = strdup(output[0]);
	param->benchmarkConfigFile = strdup(benchmarkConfigFile[0]);
	mutex_init(&param->mutex);
	if(period)
		param->period = atol(period[0]);
	param->core_num = core_num();

	if(!param->ptask)
		return 0;
	int num_task = param->ptask->num, i;
	param->events = (ctr_list_t**)malloc(sizeof(ctr_list_t*) * num_task);
	memset(param->events, 0, sizeof(ctr_list_t*) * num_task);
	for(i = 0; i < num_task; i++)
		param->events[i] = args2events(eventNames, param->period);

	if(!param->events[0] || !param->mutex)
	{
		puts("-->param->events or param->mutex init failed");
		return 0;
	}
	freeArg(taskNames);
	freeArg(eventNames);
	freeArg(period);
	freeArg(output);
	freeArg(benchmarkConfigFile);
	return 1;
}

void corun_param_free(struct corun_param *param)
{
	if(param == NULL)
		return;
	int num_task = 0, i;
	if(param->pargs)
		corun_arg_list_free(param->pargs);
	if(param->ptask)
	{
		num_task = param->ptask->num;
		task_list_free(param->ptask);
	}
	if(param->events)
	{
		for(i = 0; i < num_task; i++)
			free_event_list(param->events[i]);
		free(param->events);
	}
	munmap(param->mutex, sizeof(sem_t));
	if(param->output)
		free(param->output);
	if(param->benchmarkConfigFile)
		free(param->benchmarkConfigFile);
	free(param);
}

int task_corunning(struct corun_param *param)
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
			printf("-->attach ctrs failed with task->cmd: %s\n", task_list->task[i].cmd);
			return 0;
		}
		reset_all_event_counter(events[i]);
		enable_all_event_counter(events[i]);
	}
	for(i = 0; i < num_task; i++)
		sem_post(mutex);

	waitpid(-1, NULL, 0);
	for(i = 0; i < num_task; i++)
		disable_all_event_counter(events[i]);
	printf("-->all child process have finished!\n");
	return 1;
}

// show the data collected from events counter and save file if the output file has been assigned
void collect_data(struct corun_param *param)
{
	if(param == NULL)
		return;

	FILE *fp;
	if((fp=fopen(param->output, "a+")) != NULL)
		printf("-->succeed to open output file: %s\n", param->output);
	else
	{
		printf("-->failed to open output file:%s\n", param->output);
		return;
	}

	struct task_list *t_list = param->ptask;
	ctr_list_t **events = param->events;
	int i, j, event_num, task_num;
	uint64_t *data;
	event_num = ctrs_len(events[0]);
	task_num = t_list->num;
	data = (uint64_t*)malloc(sizeof(uint64_t) * event_num);
	memset(data, 0, sizeof(uint64_t) * event_num);

	fprintf(fp, "-------\n");
	for(j = 0; j < task_num; j++)
	{
		read_counter(data, events[j]);
		fprintf(fp, "%s ", t_list->task[j].name);
		for(i = 0; i < event_num-1; i++)
			fprintf(fp, "%lu ", data[i]);
		fprintf(fp, "%lu\n", data[i]);
	}
	fclose(fp);
	free(data);
}

void corun_param_show(struct corun_param *param)
{
	printf("-->num of core: %u\n", param->core_num);
	printf("-->period: %u\n", param->period);
	printf("-->output: %s\n", param->output);
	printf("-->config: %s\n", param->benchmarkConfigFile);
	corun_arg_list_show(param->pargs);
	task_list_show(param->ptask);
	if(param->events)
		event_list_show(param->events[0]);
}

int main(int argc, char *argv[])
{
	if(!pin_cpu(0))
		puts("-->failed to pin cpu 0");
	else
		puts("-->succeed to pin cpu 0");

	event_counter_initialize();
	printf("-->event_counter_initialize done\n");

	struct corun_param *param = (struct corun_param*)malloc(sizeof(struct corun_param));

	// initialize all parameters
	if(corun_param_init(param, argc, argv) == 0)
	{
		puts("-->corun_param_init failed");
		goto FREE;
	}
	puts("-->corun_param_init done");

	// show parameters
	corun_param_show(param);

	// run task
	if(task_corunning(param) == 0)
	{
		puts("-->task corunning failed");
		goto FREE;
	}

	// show results
	collect_data(param);
	puts("-->collect_data done");

	// free all resources
FREE:
	corun_param_free(param);
	puts("-->corun_param_free done");
	event_counter_finitial();
	puts("-->event_counter_finitial done");
}
