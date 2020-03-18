#ifndef _CACHE_H
#define _CACHE_H

//系统L3 cache参数
#define SET_NUM 2048
#define ASSOCIATION 64
#define BLOCK_SIZE 64
#define L3_CACHE_SIZE (SET_NUM * ASSOCIATION * BLOCK_SIZE)

//实验参数
#define FILE_NAME "/hugepages/set_dueling.map"
#define POINTER_LENGTH (sizeof(void*))
#define WAY_SPAN (SET_NUM * BLOCK_SIZE)
#define TIMES 10
#define HUGE_PAGE_SIZE 2 * 1024 * 1024

//cache映射地址记录结构
unsigned long *base_addr;                       //申请空间的基地址指针
unsigned long *cache_start[SET_NUM];
unsigned long *cache_zone[SET_NUM][ASSOCIATION * TIMES];

//初始化以上结构的方法
void hugepage_alloc();
void init_cache_start();
void init_cache_zone();
void fill_all_space(int association);

#endif
