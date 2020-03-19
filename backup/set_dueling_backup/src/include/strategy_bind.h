#ifndef _STRATEGY_BIND_H
#define _STRATEGY_BIND_H

#define POLICY_A_START 510
#define POLICY_A_END 580
#define POLICY_B_START 760
#define POLICY_B_END 830
#define POLICY_AREA_LENGTH 70

#define SET_NUM 2048
#define ASSOCIATION 64
#define BLOCK_SIZE 64
#define L3_CACHE_SIZE (SET_NUM * ASSOCIATION * BLOCK_SIZE)

#define FILE_NAME "/hugepages/strategy_bind.map"
#define POINTER_LENGTH (sizeof(void*))
#define WAY_SPAN (SET_NUM * BLOCK_SIZE)
#define TIMES 5

#endif
