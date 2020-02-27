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
    char *event_name;
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

ctr_t *create_ctr(const struct perf_event_attr *base_attr);
struct perf_event_attr *create_event_attr();
struct perf_event_attr *create_sample_event_attr(uint64_t sample_period);
void close_ctrs(ctr_list_t *list);

void event_counter_initialize();
void init_events_list(ctr_list_t **events_list, char *events_name[], const int len, uint64_t sample_period);
int attach_ctr(ctr_t *ctr, pid_t pid, int group_fd);
int attach_ctrs(ctr_list_t *list, pid_t pid);
void reset_all_event_counter(ctr_list_t *list);
void disable_all_event_counter(ctr_list_t *list);
void enable_all_event_counter(ctr_list_t *list);
void event_counter_finitial(ctr_list_t *list);

int ctrs_len(ctr_list_t *list);
void read_counter(uint64_t *data, ctr_list_t *list);
void events_list_show(ctr_list_t *events_list);

#endif
