#ifndef _ARGUMENT_H
#define _ARGUMENT_H

enum option
{
	PERIOD,
	EVENT,
	TASK
};

struct corun_arg
{
	enum option type;
	char *param;
};

int corun_arg_init(struct corun_arg *arg, enum option opt, char *param);
void corun_arg_free(struct corun_arg *arg);
void free_all_args(struct corun_arg *args, unsigned int num_arg);
char *corun_arg2str(const struct corun_arg *arg);
struct corun_arg* get_all_arguments(int argc, char *argv[], unsigned int *num_arg);

#endif

