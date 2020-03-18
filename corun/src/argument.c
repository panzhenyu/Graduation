#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "include/argument.h"

#define ARGQUEUE_INIT_SIZE 2

int corun_arg_init(struct corun_arg *arg, enum arg_option opt, char *param)
{
	if(arg == NULL)
		return 0;
	arg->type = opt;
	arg->param = param;
	return 1;
}

void corun_arg_free(struct corun_arg *arg)
{
	if(arg == NULL)
		return;
	free(arg->param);
	free(arg);
}

char* corun_arg2str(const struct corun_arg *arg)
{
	int num_ch = 0, init_ch = 9;
	char *s = (char*)malloc(init_ch * sizeof(char));
	switch(arg->type)
	{
		case EVENT:
			num_ch = 5;
			strcpy(s, "EVENT: ");
			break;
		case PERIOD:
			num_ch = 6;
			strcpy(s, "PERIOD: ");
			break;
		case TASK:
			num_ch = 4;
			strcpy(s, "TASK: ");
			break;
	}
	int param_len = strlen(arg->param);
	num_ch += param_len + 1;	// 1 for '\0' character
	if(num_ch > init_ch)
		s = realloc(s, num_ch);
	strcat(s, arg->param);
	return s;
}

void free_all_args(struct corun_arg *args, unsigned int num_arg)
{
	if(args == NULL || num_arg == 0)
		return;
	int i;
	for(i = 0; i < num_arg; i++)
	{
		free(args[i].param);
	}
	free(args);
}

static void helper_info()
{
}

struct corun_arg* get_all_arguments(int argc, char *argv[], unsigned int *num_arg)
{
	int ch;
	unsigned int num, max_num;
	struct corun_arg *args = NULL;
	num = max_num = 0;

	while((ch = getopt(argc, argv, "e:p:t:h")) != -1)
	{
		if(max_num == 0)
		{
			args = (struct corun_arg*)malloc(sizeof(struct corun_arg) * ARGQUEUE_INIT_SIZE);
			max_num = ARGQUEUE_INIT_SIZE;
		}
		while(num >= max_num)
		{
			max_num <<= 1;
			args = (struct corun_arg*)realloc(args, max_num * sizeof(struct corun_arg));
		}
		switch(ch)
		{
			case 'e':
				corun_arg_init(&args[num], EVENT, strdup(optarg));
				break;
			case 'p':
				// check for optarg
				corun_arg_init(&args[num], PERIOD, strdup(optarg));
				break;
			case 't':
				corun_arg_init(&args[num], TASK, strdup(optarg));
				break;
			case 'h':
				helper_info();
				break;
			default:
				printf("Unknown option: %c\n", (char)optopt);
				num--;
				break;
		}
		num++;
	}
	*num_arg = num;
	return args;
}

void show_all_arguments(const struct corun_arg *args, unsigned int num_arg)
{
	if(!args || num_arg == 0)
		return;
	int i;
	char *s;
	puts("--------Arguments--------");
	for(i = 0; i < num_arg; i++)
	{
		s = corun_arg2str(&args[i]);
		printf("%s\n", s);
		free(s);
	}
	puts("-------------------------");
	printf("\n");
}
