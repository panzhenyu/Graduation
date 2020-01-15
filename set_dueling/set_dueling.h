#ifndef _SET_DUELING_H
#define _SET_DUELING_H

#include "cache.h"

#define TEST_FILENAME "./result/test.txt"
#define RETEST_FILENAME "./result/retest.txt"

double access_list_test[SET_NUM];
double access_list_retest[SET_NUM];

void forward_set_scan(int assoc, int repeat);
void backward_set_scan(int assoc, int repeat);
void save_results();

#endif