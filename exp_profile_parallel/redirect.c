#include "redirect.h"





void redirect_to_file(const char *filename, const char *path, benchmark_data_list_t *data_list)
{
    chdir(path);
    benchmark_feature_t *cur;
    int fd;
    fd = fopen(filename, "w");
    if(fd == -1){
        perror("Can not open file\n");
        exit(EXIT_FAILURE);
    }
    fprintf(fd, "(Benchmarch_name, Strategy, Instructions, Cycle, Cache_misses, Cache_references, IPC)\n");
    for(cur = data_list->head; cur; cur = cur->next)
    {
        fprintf(fd, "(%s, strategy_A, %lld, %lld, %lld, %lld, %lf)\n", cur->benchmark, cur->strategy_a[0], cur->strategy_a[1], cur->strategy_a[2], cur->strategy_a[3], cur->strategy_a[0] / (double)cur->strategy_a[1]);
        fprintf(fd, "(%s, strategy_B, %lld, %lld, %lld, %lld, %lf)\n", cur->benchmark, cur->strategy_b[0], cur->strategy_b[1], cur->strategy_b[2], cur->strategy_b[3], cur->strategy_b[0] / (double)cur->strategy_b[1]);
    }
    close(fd);
}

void serialize_to_file(const char* filename, const char* path, profile_data_list_t* list)
{
    chdir(path);
    int fd;
    fd = fopen(filename, "w");
    if(fd == NULL)
    {
        perror("Can not open file in serialize_to_file()!\n");
        exit(EXIT_FAILURE);
    }
	profile_data_t* cur;
        int i;
	for(cur = list->head; cur; cur = cur->next)
	{
		fprintf(fd, "%s\n", cur->assign_class);
		fprintf(fd, "%s,%s,%s,%s,%s\n", "Instructions", "Cycles", "Misses", "References", "Ipc");
		for(i = 0; i < MAX_DATA_IN_EACH_PROFILE; i++)
		{
			fprintf(fd, "%d %lld,%lld,%lld,%lld,%lf\n", i, cur->data_all[i].instructions, cur->data_all[i].cycles, cur->data_all[i].misses, cur->data_all[i].references, cur->data_all[i].ipc);
		}
		//fprintf(fd, "\n\n\n");	
	}
	close(fd); 
}
