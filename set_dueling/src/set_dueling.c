#include <stdio.h>
#include <string.h>
#include "set_dueling.h"
#include "cache.h"
#include "tsc.h"

double access_set(int set, int assoc, int repeat)
{
    register int i;
    unsigned long t0, t1, total;
    void *ptr;
    int count = repeat;

    ptr = (void*)cache_zone[set][0];
    t0 = read_tsc_fenced();
    for(i = 0; i < count; i++)
        ptr = *(void **)ptr;
    t1 = read_tsc_fenced();

    return (double)(t1 - t0) / count;
}

void forward_set_scan(int assoc, int repeat)
{
    // 正向扫描每个组，每次顺序访问assoc次，每个组的访问重复repeat次，计算平均值记录于access_list_test
    int set;

    for(set = 0; set < SET_NUM; set++)
        access_list_test[set] = access_set(set, assoc, repeat);
}

void backward_set_scan(int assoc, int repeat)
{
    // 反向扫描每个组，每次顺序访问assoc次，每个组的访问重复repeat次，计算平均值记录于access_list_retest
    int set;

    for(set = SET_NUM-1; set >= 0; set--)
        access_list_retest[set] = access_set(set, assoc, repeat);
}

void save_results(char *tag)
{
    char test[50] = "../result/test_", retest[50] = "../result/retest_";
    strcat(test, tag);
    strcat(retest, tag); FILE *test_out = fopen(test,"w+");
    FILE *retest_out = fopen(retest,"w+");

    int i;
    for(i = 0; i < SET_NUM; i++)
    {
        fprintf(test_out, "%lf\n", access_list_test[i]);
        fprintf(retest_out, "%lf\n", access_list_retest[i]);
    }
    
    fclose(test_out);
    fclose(retest_out);
}

int check_route(int assoc)
{
    void *prev, *pos;
    int i, j;
    for(i = 0; i < SET_NUM; i++)
    {
        pos = cache_zone[i][0];
        for(j = 1; j < assoc; j++)
        {
            prev = pos;
            pos = *(void**)pos;
            if(pos - prev != WAY_SPAN)
                return 0;
        }
    }
    return 1;
}

static void choose_nru()
{
    #define LRU_START 577
    #define LRU_END 632
    int set;
    register int assoc, repeat;
    void *ptr;
    for(set = LRU_START; set <= LRU_END; set++)
    {
        for(repeat = 0; repeat < 100; repeat++)
        {
            ptr = (void*)cache_zone[set][0];
            for(assoc = 0; assoc < 80; assoc++)
                ptr = *(void**)ptr;
        }
    }
}

static void choose_lru()
{
    #define NRU_START 768
    #define NRU_END 832
}

void strategy_check()
{
    int set;
    void *ptr;
    unsigned long t0, t1, total;
    register int assoc, repeat;
    fill_all_space(TIMES * ASSOCIATION);
    choose_nru();
    total = 0;
    for(set = 0; set < 500; set++)
    {
        for(repeat = 0; repeat < 100; repeat++)
        {
            ptr = (void*)cache_zone[set][0];
            t0 = read_tsc_fenced();
            for(assoc = 0; assoc < 65; assoc++)
                ptr = *(void**)ptr;
            t1 = read_tsc_fenced();
            total += t1-t0;
        }
    }
    printf("set:0-499 avg_cycle:%lf\n", (double)total/3250000);
}
