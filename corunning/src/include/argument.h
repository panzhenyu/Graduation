#ifndef _ARGUMENT_H
#define _ARGUMENT_H

enum arg_option
{
	TASK,
	EVENT,
	CONFIG,
	OUTPUT,
	SAMPLE_PERIOD,
	UNKNOWN
};

struct corun_arg
{
	enum arg_option type;
	char *param;
	struct corun_arg *next;
};

// 队列首节点为空
typedef struct corun_arg* corun_arg_list;
typedef enum arg_option arg_option;

corun_arg_list corun_arg_list_init(int argc, char *argv[]);
void corun_arg_list_free(corun_arg_list args);
int corun_arg_list_length(const corun_arg_list args);
char** corun_arg_list_getByOption(const corun_arg_list args, arg_option option);
void corun_arg_list_show(const corun_arg_list args);

#endif

