#include <stdio.h>
#include "set_dueling.h"
#include "cache.h"
#include "tsc.h"

static double access_set(int set, int assoc, int repeat)
{
    int n, way, t0, t1, total = 0;
    unsigned long *ptr;

    for(n = 0; n < repeat; n++)
    {
        ptr = cache_zone[set][0];
        t0 = read_tsc();
        for(way = 0; way < assoc; way++)
            ptr = *(void **)ptr;
        t1 = read_tsc();
        total += (t1-t0);
    }
    return (double)total / (repeat * assoc);
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

void save_results()
{
    FILE *test_out = fopen(TEST_FILENAME,"w+");
    FILE *retest_out = fopen(RETEST_FILENAME,"w+");

    for(int i=0; i<SET_NUM; i++)
    {
        fprintf(test_out, "%lf\n", access_list_test[i]);
        fprintf(retest_out, "%lf\n", access_list_retest[i]);
    }
    
    fclose(test_out);
    fclose(retest_out);
}
