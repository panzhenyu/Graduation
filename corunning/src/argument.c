#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "include/argument.h"

static void helper_info()
{
	puts("helper info");
}

static struct corun_arg* corun_arg_init(enum arg_option opt, char *param)
{
	struct corun_arg *arg = (struct corun_arg*)malloc(sizeof(struct corun_arg));
	if(arg == NULL)
		return NULL;
	arg->type = opt;
	arg->param = strdup(param);
	arg->next = NULL;
	return arg;
}

static void corun_arg_free(struct corun_arg *arg)
{
	if(arg == NULL)
		return;
	free(arg->param);
}

corun_arg_list corun_arg_list_init(int argc, char *argv[])
{
	if(argc == 0 || argv == NULL)
		return NULL;
	corun_arg_list args = (corun_arg_list)malloc(sizeof(struct corun_arg));
	if(args == NULL)
		return NULL;
	
	int ch;
	args->param = NULL;
	args->next = NULL;
	struct corun_arg *var;
	enum arg_option tmpOpt;
	corun_arg_list p = args;

	while((ch = getopt(argc, argv, "t:e:c:o:p:h")) != -1)
	{
		tmpOpt = UNKNOWN;
		switch(ch)
		{
			case 't':
				tmpOpt = TASK;
				break;
			case 'e':
				tmpOpt = EVENT;
				break;
			case 'c':
				tmpOpt = CONFIG;
				break;
			case 'o':
				tmpOpt = OUTPUT;
				break;
			case 'p':
				// check for optarg
				tmpOpt = SAMPLE_PERIOD;
				break;
			case 'h':
				helper_info();
				exit(-1);
			default:
				printf("-->Unknown option: %c\n", (char)optopt);
				break;
		}
		if(tmpOpt != UNKNOWN)
		{
			var = corun_arg_init(tmpOpt, optarg);
			if(var == NULL)
			{
				perror("-->in corun_arg_list_init: corun_arg_init failed!");
				exit(-1);
			}
			p->next = var;
			p = p->next;
		}
	}
	return args;
}

void corun_arg_list_free(corun_arg_list args)
{
	struct corun_arg *p = args, *tmp;
	while(p)
	{
		tmp = p;
		p = p->next;
		free(tmp);
	}
}

int corun_arg_list_length(const corun_arg_list args)
{
	if(args == NULL)
		return -1;
	int length = 0;
	struct corun_arg *p = args->next;
	while(p)
	{
		length++;
		p = p->next;
	}
	return length;
}

// 返回指针数组，最后一个元素值为NULL
char** corun_arg_list_getByOption(const corun_arg_list args, arg_option option)
{
	int length = corun_arg_list_length(args);
	if(length <= 0)
		return NULL;

	int match = 0;
	char **result = (char**)malloc(sizeof(char*) * (length + 1));
	struct corun_arg *p = args->next;

	while(p)
	{
		if(p->type == option)
			result[match++] = strdup(p->param);
		p = p->next;
	}
	if(match != 0)
	{
		result = (char**)realloc((void*)result, sizeof(char*) * (match + 1));
		result[match] = NULL;
		return result;
	}
	free(result);
	return NULL;
}

static char* corun_arg2str(const struct corun_arg *arg)
{
	int num_ch = 0, init_ch = 9;
	char *s = (char*)malloc(init_ch * sizeof(char));
	switch(arg->type)
	{
		case EVENT:
			num_ch = 7;
			strcpy(s, "EVENT: ");
			break;
		case SAMPLE_PERIOD:
			num_ch = 8;
			strcpy(s, "PERIOD: ");
			break;
		case TASK:
			num_ch = 6;
			strcpy(s, "TASK: ");
			break;
		case OUTPUT:
			num_ch = 8;
			strcpy(s, "OUTPUT: ");
			break;
		case CONFIG:
			num_ch = 8;
			strcpy(s, "CONFIG: ");
			break;
	}
	int param_len = strlen(arg->param);
	num_ch += param_len + 1;	// 1 for '\0' character
	if(num_ch > init_ch)
		s = realloc(s, num_ch);
	strcat(s, arg->param);
	return s;
}

void corun_arg_list_show(const corun_arg_list args)
{
	int length = corun_arg_list_length(args);
	if(length <= 0)
		return;

	int i;
	char *s;
	struct corun_arg *p = args->next;

	puts("--------Arguments--------");
	while(p)
	{
		s = corun_arg2str(p);
		printf("%s\n", s);
		free(s);
		p = p->next;
	}
	puts("-------------------------\n");
}
