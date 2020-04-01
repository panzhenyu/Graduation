#ifndef EXP_DATA_H
#define EXP_DATA_H


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdint.h>
#include <errno.h>
#include "perf_counter.h"


#define MAX_DATA_IN_EACH_PROFILE 1
/*
//记录事件发生次数的结构
struct event_data{
    uint64_t instructions;
    uint64_t cycle;
    uint64_t misses;
    uint64_t access;
};
*/

//每一个bubble大小下要测量的事件数据
typedef struct events_data_item{
    uint64_t instructions;
    uint64_t cycles;
    uint64_t misses;
    uint64_t references;
    double ipc;
} events_data_item_t;

//一个benchmark在一种指定状态下需要测量的数据
typedef struct profile_data{
    char *assign_class;
    events_data_item_t data_all[MAX_DATA_IN_EACH_PROFILE];
    struct profile_data *next;
} profile_data_t;

//一个benchmark在所有指定状态下的数据（不指定， 指定A， 指定B， worst_case）
typedef struct profile_data_list{
    profile_data_t *head;
    profile_data_t *tail;
} profile_data_list_t;

//记录其中一个benchmark在（a, b）两个替换策略下的特性结构
typedef struct benchmark_feature{
    char *benchmark;
    uint64_t *strategy_a;
    uint64_t *strategy_b;
    struct benchmark_feature *next;
}benchmark_feature_t;


//记录性能数据的队列，一个元素对应一个benchmark
typedef struct {
    struct benchmark_feature *head;
    struct benchmark_feature *tail;
}benchmark_data_list_t;


//创建一个记录数据的元素
benchmark_feature_t *create_Item(const char *name);

//记录来自策略a的数据
void record_data_strategy_a(benchmark_feature_t *item, ctr_list_t *list);

//记录来自策略b的数据
void record_data_strategy_b(benchmark_feature_t *item, ctr_list_t *list);

//数据元素加到链表中（方便下一步序列化）
void add_Item(benchmark_data_list_t *data_list, benchmark_feature_t *item);




//创建一个指定状态下的实例
profile_data_t* create_profile_data(const char* assign_class);

//把一个指定状态下的数据加入总数据链表尾部
void add_profile_data(profile_data_list_t* list, profile_data_t* item);

//读取性能计数器相对应的文件描述符，读取数据
void read_sample_data(profile_data_t* data, int idx, ctr_list_t* list);

//生成worst_case
void generate_worst_case(profile_data_list_t* list, const char* assign_class);

//析构总数据链表
void destruct_profile_data_list(profile_data_list_t* list);

#endif
