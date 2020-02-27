#include <stdio.h>
#include <stdlib.h>
#include "include/corun_args.h"

int main(int argc, char *argv[])
{
	unsigned int num_arg = 0;
	struct corun_arg *args = get_all_arguments(argc, argv, &num_arg);
	printf("num_arg = %u\n", num_arg);
	int i;
	for(i = 0; i < num_arg; i++)
	{
		char *s = corun_arg2str(&args[i]);
		printf("%s\n", s);
		free(s);
	}
	free_all_args(args, num_arg);
	return 0;
}
