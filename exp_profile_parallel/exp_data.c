#include "exp_data.h"


benchmark_feature_t *create_Item(const char *name)
{
    benchmark_feature_t *bft;
    bft = malloc(sizeof(benchmark_feature_t));
    if(bft == NULL)
    {
        perror("Can not alloc!");
        exit(EXIT_FAILURE);
    }
    memset(bft, 0, sizeof(benchmark_feature_t));
    bft->benchmark = name;
    bft->strategy_a = NULL;
    bft->strategy_b = NULL;
    bft->next = NULL;
    return bft;
}


void record_data_strategy_a(benchmark_feature_t *item, ctr_list_t *list)
{
    int len;
    len = ctrs_len(list);
    uint64_t *data;
    data = malloc(sizeof(uint64_t) * len);
    if(data == NULL)
    {
        perror("Can not alloc for sizeof(uint64_t) * len");
        exit(EXIT_FAILURE);
    }
    memset(data, 0, sizeof(uint64_t) * len);
    read_counter(data, list);
    item->strategy_a = data;
}


void record_data_strategy_b(benchmark_feature_t *item, ctr_list_t *list)
{
    int len;
    len = ctrs_len(list);
    uint64_t *data;
    data = malloc(sizeof(uint64_t) * len);
    if(data == NULL)
    {
        perror("Can not alloc for sizeof(uint64_t) * len");
        exit(EXIT_FAILURE);
    }
    memset(data, 0, sizeof(uint64_t) * len);
    read_counter(data, list);
    item->strategy_b = data;
}

void add_Item(benchmark_data_list_t *data_list, benchmark_feature_t *item)
{
    if(!data_list->head)
        data_list->head = data_list->tail = item;
    else{
        data_list->tail->next = item;
        data_list->tail = item;
    }
}


profile_data_t* create_profile_data(const char* assign_class)
{
    profile_data_t* item;
    item = (profile_data_t*)malloc(sizeof(profile_data_t));
    if(item == NULL)
    {
        perror("Can not alloc!\n");
        exit(EXIT_FAILURE);
    }
    memset(item, 0, sizeof(profile_data_t));
    item->assign_class = assign_class;
    item->next = NULL;
    return item;
}


void add_profile_data(profile_data_list_t* list, profile_data_t* item)
{
    if(list->head == NULL)
        list->head = list->tail = item;
    else
    {
        list->tail->next = item;
        list->tail = item;
    }
}


void read_sample_data(profile_data_t* data, int idx, ctr_list_t* list)
{
    int i;
    ctr_t* cur;
    uint64_t events[4];
    ssize_t ret;
    for(i = 0, cur = list->head; cur; cur = cur->next, i++)
    {
        ret = read(cur->fd, &events[i], sizeof(uint64_t));
        if(ret == 0)
        {
            perror("got EOF while reading counter\n");
            exit(EXIT_FAILURE);
        }
        else if(ret != sizeof(uint64_t))
            fprintf(stderr,
                    "Warning: Got short read. Expexted %i bytes,"
                    "but got %i bytes.\n", sizeof(uint64_t), ret);
    }
    //printf("instruct id %lld\n", events[0]);
    data->data_all[idx].instructions = events[0];
    //printf("cycles is %lld\n", events[1]);
    data->data_all[idx].cycles = events[1];
    //printf("misses is %lld\n", events[2]);
    data->data_all[idx].misses = events[2];
    //printf("references is %lld\n", events[3]);
    data->data_all[idx].references = events[3];
    //printf("ipc is %ll\n", (double)events[0]/events[1]);
    data->data_all[idx].ipc = (double)events[0]/events[1];
}


void generate_worst_case(profile_data_list_t* list, const char* assign_class)
{
    profile_data_t* worst_case = create_profile_data(assign_class);
    int i;
    profile_data_t* best = list->head;
    profile_data_t* policy_a = list->head->next;
    profile_data_t* policy_b = list->head->next->next;
    for(i = 0; i < MAX_DATA_IN_EACH_PROFILE; i++)
    {
		worst_case->data_all[i].instructions = best->data_all[i].instructions + ((policy_a->data_all[i].instructions - best->data_all[i].instructions)+(policy_b->data_all[i].instructions - best->data_all[i].instructions));
		
		worst_case->data_all[i].cycles = best->data_all[i].cycles + ((policy_a->data_all[i].cycles - best->data_all[i].cycles)+(policy_b->data_all[i].cycles - best->data_all[i].cycles));

		worst_case->data_all[i].misses = best->data_all[i].misses + ((policy_a->data_all[i].misses - best->data_all[i].misses)+(policy_b->data_all[i].misses - best->data_all[i].misses));

		worst_case->data_all[i].references = best->data_all[i].references + ((policy_a->data_all[i].references - best->data_all[i].references)+(policy_b->data_all[i].references - best->data_all[i].references));

		worst_case->data_all[i].ipc = best->data_all[i].ipc + ((policy_a->data_all[i].ipc - best->data_all[i].ipc)+(policy_b->data_all[i].ipc - best->data_all[i].ipc)); 
    }
    list->tail->next = worst_case;
    list->tail = worst_case;
}



void destruct_profile_data_list(profile_data_list_t* list)
{
    profile_data_t* cur;
    for(cur = list->head; cur; cur = cur->next)
    {
        free(cur);
    }
    list->head = list->tail = NULL;
}

