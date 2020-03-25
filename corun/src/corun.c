#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "include/corun.h"

static struct task_list* args2tasks(struct corun_arg *args, unsigned int num_arg)
{
	if(!args || num_arg == 0)
		return NULL;
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
	if(!args || num_arg == 0)
		return DEFAULT_PERIOD;
	int i;
	for(i = 0; i < num_arg; i++)
	{
		if(args[i].type == PERIOD)
			return atol(args[i].param);
	}
	return DEFAULT_PERIOD;
}

static ctr_list_t* args2events(struct corun_arg *args, unsigned int num_arg, unsigned int period)
{
	if(!args || num_arg == 0)
		return NULL;
	int i, j;
	char **events_name = (char**)malloc(sizeof(char*) * num_arg);
	for(i = 0, j = 0; i < num_arg; i++)
	{
		if(args[i].type == EVENT)
			events_name[j++] = strdup(args[i].param);
	}
	ctr_list_t *events = NULL;
	init_events_list(&events, events_name, j, period);

	for(i = 0; i < j; i++)
		free(events_name[i]);
	free(events_name);
	return events;
}

static char* args2output(struct corun_arg *args, unsigned int num_arg)
{
	if(!args || num_arg == 0)
		return NULL;
	int i;
	for(i = 0; i < num_arg; i++)
	{
		if(args[i].type == OUTPUT)
			return strdup(args[i].param);
	}
	return NULL;
}

static unsigned long args2time(struct corun_arg *args, unsigned int num_arg)
{
	if(!args || num_arg == 0)
		return DEFAULT_TIME;
	int i;
	for(i = 0; i < num_arg; i++)
	{
		if(args[i].type == TIME)
			return atol(args[i].param);
	}
	return DEFAULT_TIME;
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
	write(fd, "init", 4);
	*ptr = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if(*ptr == MAP_FAILED)
	{
		printf("mmap failed\n");
		*ptr = NULL;
		return;
	}
	if(sem_init(*ptr, 1, 0) < 0)
	{
		printf("sem_init failed\n");
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
	param->period = args2period(param->pargs, param->num_arg);
	param->time = args2time(param->pargs, param->num_arg);
	param->core_num = core_num();
	param->ptask = args2tasks(param->pargs, param->num_arg);

	if(!param->ptask)
		return 0;
	num_task = param->ptask->num;

	param->events = (ctr_list_t**)malloc(sizeof(ctr_list_t*) * num_task);
	memset(param->events, 0, sizeof(ctr_list_t*) * num_task);
	for(i = 0; i < num_task; i++)
		param->events[i] = args2events(param->pargs, param->num_arg, param->period);

	mutex_init(&param->mutex);
	if(!param->events[0])
	{
		puts("param->events[0] init failed");
		return 0;
	}
	if(!param->mutex)
	{
		puts("param->mutex init failed");
		return 0;
	}
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
	munmap(param->mutex, sizeof(sem_t));
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
			printf("attach ctrs failed with task->cmd: %s\n", task_list->task[i].cmd);
			return 0;
		}
		reset_all_event_counter(events[i]);
		enable_all_event_counter(events[i]);
	}
	for(i = 0; i < num_task; i++)
		sem_post(mutex);
	if(param->time == 0)
        usleep(param->time * 1000);
    else
	   waitpid(-1, NULL, 0);
	for(i = 0; i < num_task; i++)
		disable_all_event_counter(events[i]);
	printf("all child process have finished!\n");
	return 1;
}

// show the data collected from events counter and save file if the output file has been assigned
void collect_data(struct corun_param *param)
{
	if(param == NULL)
		return;
	char *output = NULL;
	uint64_t *data = NULL;
	FILE *fp = NULL;
	int i, j, event_num, task_num;

	output = args2output(param->pargs, param->num_arg);
	if(output != NULL && (fp=fopen(output, "w+")) != NULL)
		printf("succeed to open output file: %s\n", output);
	else
		printf("failed to open output file:%s\n", output);

	struct task_list *t_list = param->ptask;
	ctr_list_t **events = param->events;
	event_num = ctrs_len(events[0]);
	task_num = t_list->num;

	data = (uint64_t*)malloc(sizeof(uint64_t) * event_num);
	memset(data, 0, sizeof(uint64_t) * event_num);

	printf("----------Results----------\n");
	for(j = 0; j < task_num; j++)
	{
		read_counter(data, events[j]);
		printf("%s: ", t_list->task[j].cmd);
		for(i = 0; i < event_num-1; i++)
			printf("%lu ", data[i]);
		printf("%lu\n", data[i]);

		if(fp)
		{
			fprintf(fp, "%s: ", t_list->task[j].cmd);
			for(i = 0; i < event_num-1; i++)
				fprintf(fp, "%lu ", data[i]);
			fprintf(fp, "%lu\n", data[i]);
		}
	}
	printf("---------------------------\n");
	if(fp)
		fclose(fp);
	if(output)
		free(output);
	free(data);
}

void corun_param_show(struct corun_param *param)
{
	printf("num of core: %u\n", param->core_num);
	printf("period: %u\n", param->period);
	printf("time: %u\n", param->time);
	show_all_arguments(param->pargs, param->num_arg);
	task_list_show(param->ptask);
	if(param->events)
		event_list_show(param->events[0]);
}

int main(int argc, char *argv[])
{
    if(!pin_cpu(0))
        puts("failed to pin cpu 0");
    else
        puts("succeed to pin cpu 0");

	event_counter_initialize();
	printf("event_counter_initialize done\n");

	struct corun_param *param = (struct corun_param*)malloc(sizeof(struct corun_param));

	// initialize all parameters
	if(corun_param_init(param, argc, argv) == 0)
	{
		puts("corun_param_init failed");
		goto FREE;
	}
	puts("corun_param_init done");

	// show parameters
	corun_param_show(param);

	// run task
	if(task_corunning(param) == 0)
	{
		puts("task corunning failed");
		goto FREE;
	}

	// show results
	collect_data(param);
	puts("collect_data done");

	// free all resources
FREE:
	corun_param_free(param);
	puts("corun_param_free done");
	free(param);
	event_counter_finitial();
	puts("event_counter_finitial done");
}

