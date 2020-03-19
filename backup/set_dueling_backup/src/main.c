#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "cache.h"
#include "pincpu.h"
#include "set_dueling.h"
#include "process_prio.h"

int main(int argc, char *argv[])
{
    int i, assoc;
    if(argc != 2)
    {
        printf("Invalid arguments!\n");
        exit(-1);
    }
    assoc = atoi(argv[1]);
    printf("assoc: %d\n", assoc);

    hugepage_alloc();
    puts("hugepage_alloc finished!");

    init_cache_start();
    puts("init_cache_start finished!");
    
    init_cache_zone();
    puts("init_cache_zone finished!");

    fill_all_space(assoc);
    puts("fill_all_space finished!");

    if(check_route(assoc))
        puts("route built successfully!");
    else
        puts("route built failed!");

    if(pin_cpu(1))
        puts("succeed to pin cpu 1!");
    else
        puts("failed to pin cpu 1!");
    if(set_fifo())
        puts("succeed to set real time process!");
    else
        puts("failed to set real time process!");

    forward_set_scan(assoc, 10000);
    puts("forward_set_scan finished!");

    backward_set_scan(assoc, 10000);
    puts("backward_set_scan finished!");

    save_results(argv[1]);
    puts("save_results finished!");

//    strategy_check();
    return 0;
}
