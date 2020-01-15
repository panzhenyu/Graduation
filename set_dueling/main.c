#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "cache.h"
#include "set_dueling.h"

int main(int argc, char *argv[])
{
    int i, assoc;
    if(argc != 2 && !isdigit(argv[1]))
    {
        printf("Invalid arguments!\n");
        exit(-1);
    }
    assoc = atoi(argv[1]);

    hugepage_alloc();
    puts("hugepage_alloc finished!");

    init_cache_start();
    puts("init_cache_start finished!");
    
    init_cache_zone();
    puts("init_cache_zone finished!");

    fill_all_space();
    puts("fill_all_space finished!");
    
    for(i = 0; i < 5; i++)
        forward_set_scan(assoc, 20);
    puts("forward_set_scan finished!");

    for(i = 0; i < 5; i++)
    backward_set_scan(assoc, 20);
    puts("backward_set_scan finished!");

    save_results();
    puts("save_results finished!");
}