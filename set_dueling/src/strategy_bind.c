#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include "include/pincpu.h"
#include "include/process_prio.h"
#include "include/strategy_bind.h"

unsigned long *base_addr;
unsigned long *cache_start[SET_NUM];
unsigned long *cache_zone[SET_NUM][ASSOCIATION];

void hugepage_alloc()
{
	// 映射一个TIMES倍cache大小的内存，返回映射内存区的起始虚拟地址，并根据FIEL_NAME文件建立一个共享文件映射
	int fd = open(FILE_NAME, O_CREAT | O_RDWR);
	if(fd < 0)
	{
		perror("Failed to open file");
		exit(0);
	}

	base_addr = (unsigned long *)mmap(0UL, TIMES*L3_CACHE_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
	if(base_addr == MAP_FAILED)
	{
		perror("Failed to mmap file\n");
		unlink(FILE_NAME);
		exit(1);
	}
}

void init_cache_start()
{
    // 初始化cache_start
    int set, interval = BLOCK_SIZE / POINTER_LENGTH;

    cache_start[0] = base_addr;
    for(set = 1; set < SET_NUM; set++)
        cache_start[set] = cache_start[set - 1] + interval;
}

void init_cache_zone()
{
    // 初始化cache_zone
    int set, assoc, total_a = TIMES*ASSOCIATION, step = WAY_SPAN / POINTER_LENGTH;
    unsigned long *addr;

    for(set = 0; set < SET_NUM; set++)
    {
        addr = cache_start[set];
        for(assoc = 0; assoc < total_a; assoc++, addr+=step)
        {
            cache_zone[set][assoc] = addr;
        }
    }
}

void fill_all_space(int association)
{
    // 填充cache_zone指向的内存区域
    int set, assoc, total_a = association;

    for(set = 0; set < SET_NUM; set++)
    {
        *(cache_zone[set][total_a-1]) = (unsigned long)cache_zone[set][0];
        for(assoc = 0; assoc < total_a-1; assoc++)
        {
            *(cache_zone[set][assoc]) = (unsigned long)cache_zone[set][assoc+1];
        }
    }
}
void delay(int time)
{
	while(time--);
}

void access_set(int set, int repeat)
{
	void *ptr;
	register int i;
	int count = repeat;

	ptr = (void*)cache_zone[set][0];
	for(i = 0; i < count; i++)
		ptr = *(void **)ptr;
}

void bind(int begin, int end)
{
	while(1)
		access_set((begin+end)/2, 1000);
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		puts("usage: ./strategy_bind A/B");
		exit(1);
	}
	hugepage_alloc();
	puts("hugepage_alloc done");
	init_cache_start();
	puts("init_cache_start done");
	init_cache_zone();
	puts("init_cache_zone done");
	fill_all_space(80);
	puts("fill_all_space done");
	if(pin_cpu(3))
		printf("succeed to pin cpu 3\n");
	else
	{
		puts("failed to pin cpu 3");
		exit(-1);
	}
	if(set_fifo())
		puts("succeed to set real time process");
	else
	{
		puts("failed to set real time process");
		exit(-2);
	}
	if(!strcmp(argv[1], "A"))
	{
		puts("begin to bind strategy A");
		bind(POLICY_B_START, POLICY_B_END);
	}
	else if(!strcmp(argv[1], "B"))
	{
		puts("begin to bind strategy B");
		bind(POLICY_A_START, POLICY_A_END);
	}
}
