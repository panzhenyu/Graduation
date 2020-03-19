#ifndef _SET_DUELING_H
#define _SET_DUELING_H

#include "cache.h"

#define RESULT_HOME "../result"

double access_list_test[SET_NUM];
double access_list_retest[SET_NUM];

double access_set(int set, int assoc, int repeat);
void forward_set_scan(int assoc, int repeat);
void backward_set_scan(int assoc, int repeat);
void save_results(char *tag);
int check_route();
void strategy_check();

#endif
