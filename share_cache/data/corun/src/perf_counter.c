#include <string.h>
#include "include/perf_counter.h"

ctr_t *create_ctr(const struct perf_event_attr *base_attr)
{
    ctr_t *ctr;
    ctr = (ctr_t*)malloc(sizeof(ctr_t));
    if(!ctr)
        return NULL;
    ctr->attr = *base_attr;
    ctr->fd = -1;
    ctr->next = NULL;
    return ctr; 
}

struct perf_event_attr *create_event_attr()
{
    struct perf_event_attr *attr;
    attr = (struct perf_event_attr*)malloc(sizeof(struct perf_event_attr));
    memset(attr, 0, sizeof(struct perf_event_attr));
    attr->type = PERF_TYPE_HARDWARE;
    attr->size = PERF_ATTR_SIZE_VER0;
   // attr->read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING | PERF_FORMAT_GROUP;
    attr->exclude_kernel = 1;
    attr->exclude_hv = 1;
    attr->exclude_idle = 1;
    attr->disabled = 1;
    return attr;
}

struct perf_event_attr *create_sample_event_attr(uint64_t sample_period)
{
    struct perf_event_attr *attr = create_event_attr();
    attr->sample_period = sample_period;
    attr->sample_type = PERF_SAMPLE_IP;
    attr->wakeup_events = 1;
    return attr;
}

void init_events_list(ctr_list_t **events_list, char *events_name[], const int len, uint64_t sample_period)
{
    if(!events_name || len == 0)
	return;
    *events_list = (ctr_list_t*)malloc(sizeof(ctr_list_t));
    memset(*events_list, 0, sizeof(ctr_list_t));
    int idx_item;
    for(idx_item = 0; idx_item < len; idx_item++)
    {
        struct perf_event_attr *attr;
        if(idx_item == 0)
            attr = create_sample_event_attr(sample_period);
        else
            attr = create_event_attr();
        ctr_t *ctr = create_ctr(attr);
        ctr->event_name = strdup(events_name[idx_item]);
        int ret;
        ret = pfm_get_perf_event_encoding(ctr->event_name, PFM_PLM3, &ctr->attr, NULL, NULL);
        if(ret != PFM_SUCCESS)
        {
            fprintf(stderr, "Not a valid event: %s\n"
                       "pfm_get_perf_encoding return: %s\n", ctr->event_name, (pfm_strerror(ret)));
            exit(1);
        }
        //把事件加入事件列表
        if(!(*events_list)->head)
            (*events_list)->head = (*events_list)->tail = ctr;
        else{
            (*events_list)->tail->next = ctr;
            (*events_list)->tail = ctr;
        }
    }
}

void close_ctrs(ctr_list_t *list)
{
    ctr_t *cur;
    for(cur = list->head; cur; cur = cur->next)
    {
        if(cur->fd != -1)
        {
            close(cur->fd);
            cur->fd  = -1;
        }

    }
}

int attach_ctr(ctr_t *ctr, pid_t pid, int group_fd)
{
    assert(ctr->fd == -1);

    //设置与事件对应的文件描述符，之后就可以通过读写文件来操作性能计数器了
    ctr->fd = perf_event_open(&ctr->attr, pid, -1, group_fd, 0);

    if(ctr->fd == -1)
    {
        perror("Failed to attach performance counter");
        return -1;
    }
    return ctr->fd;
}

int attach_ctrs(ctr_list_t *list, pid_t pid)
{
    ctr_t *cur;
    for(cur = list->head; cur; cur = cur->next)
    {
        attach_ctr(cur, pid, -1);
        if(cur->fd == -1)
        {
            close_ctrs(list);
            return -1;
        }
    }
    return 0;
}

void reset_all_event_counter(ctr_list_t *list)
{
    ctr_t *cur;
    for(cur = list->head; cur; cur = cur->next)
    {
        EXPECT_ERRNO(-1 != ioctl(cur->fd, PERF_EVENT_IOC_RESET, 0));
    }
}

void disable_all_event_counter(ctr_list_t *list)
{
    ctr_t *cur;
    for(cur = list->head; cur; cur = cur->next)
    {
        EXPECT_ERRNO(-1 != ioctl(cur->fd, PERF_EVENT_IOC_DISABLE, 0));
    }
}

void enable_all_event_counter(ctr_list_t *list)
{
    ctr_t *cur;
    for(cur = list->head; cur; cur = cur->next)
    {
        EXPECT_ERRNO(-1 != ioctl(cur->fd, PERF_EVENT_IOC_ENABLE, 0));
    }
}

void event_counter_initialize()
{
    pfm_initialize();
}

void event_counter_finitial(ctr_list_t *list)
{
    if(!list)
        return;
    close_ctrs(list);
    ctr_t *cur = list->head, *var;
    while(cur)
    {
        var = cur;
        cur = cur->next;
	if(var->event_name)
		free(var->event_name);
        free(var);
    }
    free(list);
    pfm_terminate();
}

int ctrs_len(ctr_list_t *list)
{
    int count = 0;
    ctr_t *cur;
    for(cur = list->head; cur; cur = cur->next)
        count++;
    return count;
}

void read_counter(uint64_t *data, ctr_list_t *list)
{
    int idx_event;
    ctr_t *cur;
    ssize_t ret;
    for(idx_event = 0, cur = list->head; cur; cur = cur->next, idx_event++)
    {
        ret = read(cur->fd, &data[idx_event], sizeof(uint64_t));
        if(ret == 0){
            perror("got EOF while reading counter\n");
            exit(EXIT_FAILURE);
        }else if(ret != sizeof(uint64_t))
            fprintf(stderr,
                    "Warning: Got short read. Expected %lu bytes, "
                    "but got %li bytes.\n",
                    sizeof(uint64_t), ret);
    }
}

void events_list_show(ctr_list_t *events_list)
{
	if(!events_list)
		return;
	puts("--------EVENTS--------");
	struct ctr *cur;
	for(cur = events_list->head; cur; cur = cur->next)
		printf("fd: %-5devent_name: %s\n", cur->fd, cur->event_name);
	puts("----------------------");
	printf("\n");
}
