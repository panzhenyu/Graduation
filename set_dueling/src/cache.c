#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "cache.h"

void hugepage_alloc()
{
    // 映射一个TIMES倍于cache大小的内存，返回映射内存区的起始虚拟地址，并根据FIEL_NAME文件建立一个共享文件映射
    int fd = open(FILE_NAME, O_CREAT | O_RDWR);
    if(fd < 0)
    {
        perror("Failed to open file");
        exit(0);
    }

    base_addr = (unsigned long *)mmap(0UL, TIMES * L3_CACHE_SIZE, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
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
    int set, assoc, total_a = TIMES * ASSOCIATION, step = WAY_SPAN / POINTER_LENGTH;
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
