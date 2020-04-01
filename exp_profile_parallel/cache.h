#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "pagemap.h"


/*
 * 3000系列机器cache的结构：
 * （1）四核分四个slice
 * （2）每个slice 16 路，一共16 * 4 = 64 路， 一共2048组
 * （2）总大小64B * 2048 * 64 = 8MB
 * （3）(set， way， size） = （2048, 64, 8MB）
 */


//系统L3_cache参数
#define SET_NUM 2048
#define ASSOCIATION 64
#define BLOCKSIZE 64  //2的6次
#define L3_CACHE_SIZE (8 * 1024 * 1024)

//大致的策略区位置(策略A, 策略B)~（513~577 & 1029~1086, 769~833 & 1290~1340）
#define POLICY_LOW_FIRST_START 516
#define POLICY_LOW_SECOND_START 1032
#define POLICY_HIGH_FIRST_START 772
#define POLICY_HIGH_SECOND_START 1290


//包含两个专用组的区间（bubble访问集需要越过这两个区间，因为bubble不能访问专用组）
#define PSEUDO_LOW_START 510
#define PSEUDO_LOW_END 580
#define PSEUDO_HIGH_START 760
#define PSEUDO_HIGH_END 830

//bublle访问集的长度 留一个串联带宽挤压
#define BUBBLE_ACCESS_LENGTH (2048-((580-510+1)+(830-760+1))-20)*ASSOCIATION*3

//实验使用的策略区的长度
#define POLICY_NUM 70

//unsigned long类型的长度（一般为4字节）
#define POINTER_LENGTH sizeof(unsigned long)

//bubble_size的改变跨度(16384个block=1mb)
#define BUBBLE_SPAN 16384    //2的14次


//实验参数
#define WAYSPAN (L3_CACHE_SIZE / ASSOCIATION / POINTER_LENGTH)  //每一路的步进

#define CACHESPAN (L3_CACHE_SIZE / POINTER_LENGTH)   //整个cache的步进

//申请空间大小(3倍于cache大小)
#define SIZE 8 * 10 * 1024 * 1024
#define HUGE_PAGE_SIZE 4 * 1024 * 1024
#define FILE_NAME "/mnt/hugepage"
#define ADDR (void *)(0x0UL)


//cache映射地址记录结构
unsigned long *gb_base_addr;  //申请空间的基地址指针
unsigned long *gb_cache_zone[SET_NUM][ASSOCIATION * 3];   //三倍于cache大小的所申请的地址空间的结构
unsigned long *gb_cache_start[SET_NUM];

//bubble访问集
unsigned long *gb_bubble_zone[BUBBLE_ACCESS_LENGTH];


//关于策略区的结构
unsigned long *gb_policy_low_first[POLICY_NUM];
unsigned long *gb_policy_low_second[POLICY_NUM];
unsigned long *gb_policy_high_first[POLICY_NUM];
unsigned long *gb_policy_high_second[POLICY_NUM];


//初始化以上结构的方法
void init_cache_zone(int init_start);
void init_policy_pointer_chase();

//初始化bubble访问区
void init_bubble_zone();

//连续内存申请和初始化
void hugepage_alloc();
void fill_all_space();


//策略指定
void assign_low_policy(double delay);
void assign_high_policy(double delay);
void assign_follow_policy(double delay);
//memory——stress
void memory_bubble_A(int bubble_local);
void memory_bubble_B(int bubble_local);
void memory_bubble_N(int bubble_local);




#endif
