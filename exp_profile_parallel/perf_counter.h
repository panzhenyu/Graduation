#ifndef PERF_COUNTER_H
#define PERF_COUNTER_H

#ifndef PFM_INC
#include <perfmon/pfmlib_perf_event.h>
#define PFM_INC
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>

#include <argp.h>

#include "expect.h"





//事件结构
typedef struct ctr {
    struct perf_event_attr attr;
    const char *event_name;
    int fd;
    struct ctr *next;
} ctr_t;


//读取数据事件计数的结构
typedef struct {
    uint64_t nr;
    uint64_t time_enabled;
    uint64_t time_running;
    struct ctr_data {
        uint64_t val;
    } ctr[];
} read_format_t;


//事件链结构
typedef struct {
    struct ctr *head;
    struct ctr *tail;
} ctr_list_t;


//事件相关的函数
ctr_t *create_ctr(const struct perf_event_attr *base_attr);   //创建事件结构
struct perf_event_attr *create_event_attr(uint64_t perf_event_id);   //创建事件属性
struct perf_event_attr *create_sample_event_attr(uint64_t perf_event_id, uint64_t sample_period);
void init_events_list(ctr_list_t **events_list, const uint64_t *events_id, const char **events_name, int len, uint64_t sample_period);   //初始化事件，把需要测量的事件加入事件列表
void close_ctrs(ctr_list_t *list);   //关闭事件的文件描述符
int attach_ctr(ctr_t *ctr, pid_t pid, int group_fd);   //为事件绑定文件描述符（绑定到特定进程中）
int attach_ctrs(ctr_list_t *list, pid_t pid);   //为事件列表中的所有事件绑定文件描述符





//重置事件计数器
void reset_all_event_counter(ctr_list_t *list);
//停止事件计数器er(ctr_list_t *list);
void disable_all_event_counter(ctr_list_t *list);
//使能事件计数器
void enable_all_event_counter(ctr_list_t *list);
//初始化性能计数器
void event_counter_initialize();
//析构性能计数器
void event_counter_finitial(ctr_list_t *list);
//计算事件链长度
int ctrs_len(ctr_list_t *list);
//读性能计数器值
void read_counter(uint64_t *data, ctr_list_t *list);




#endif
