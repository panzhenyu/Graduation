#include "cache.h"


void hugepage_alloc()
{
    int fd = open(FILE_NAME, O_CREAT | O_RDWR);//以创建方式打开一个文件，并且可读可写
    if (fd < 0)
    {
        printf("open file:FILE_NAME failed \n");
        exit(0);
    }
//映射一个10倍于cache大小的内存，返回映射内存区的起始虚拟地址，并根据/mnt/hugepage文件建立一个共享文件映射
    gb_base_addr = (unsigned long *)mmap(ADDR/*0x0UL*/, SIZE/*10*8MB*/, PROT_WRITE|PROT_READ, MAP_SHARED, fd/*/mnt/hugepage*/, 0);
    printf("*********\n");
    if (gb_base_addr == MAP_FAILED)
    {
        perror("mmap");
        unlink(FILE_NAME);
        exit(1);
    }
}



void fill_all_space()
{
    int idx_set/*set的index*/, idx_assoc/**/;
    unsigned long long tmp_PFN;

    int idx_hugepage_num;//每个hugapage的相对序号

    gb_cache_start[0] = gb_base_addr;
    for(idx_set = 1; idx_set < SET_NUM; idx_set++)
    {
        gb_cache_start[idx_set] = gb_cache_start[idx_set - 1] + BLOCKSIZE/POINTER_LENGTH;
    }
//这个循环主要是往每条链上的blocksize大小上的前4B空间填0
    for(idx_set = 0; idx_set < SET_NUM; idx_set++)
    {
        for(idx_assoc = 0; idx_assoc < 10 * ASSOCIATION; idx_assoc++)
        {
            //申请了5倍相联度大小的区域（？？为什么不是10倍？？）
            *(gb_cache_start[idx_set] + WAYSPAN * idx_assoc) = 0;
        }
    }

    sprintf(path_buf, "/proc/self/pagemap");
    int loc=0;
    int len=0;
    unsigned long long pre_address=0;
    for(idx_hugepage_num = 0; idx_hugepage_num < 20; idx_hugepage_num++)
    {
        tmp_PFN = read_pagemap(path_buf, gb_base_addr + BLOCKSIZE/POINTER_LENGTH * (2048 * 64 /2) * idx_hugepage_num);
        printf("%d:%lld\n", idx_hugepage_num, tmp_PFN); //输出分配的hugepage的物理地址
        //计算偏移地址
        if(len<6)
        {
            if((tmp_PFN-pre_address)!=1024)
            {
                loc=idx_hugepage_num;
                len=0;
            }
            else len++;
        }
        pre_address=tmp_PFN;
    }
    //!!!!!!!!!偏移
    int offset=loc;
    gb_base_addr=gb_base_addr + BLOCKSIZE/POINTER_LENGTH * (2048 * 64 /2) * offset;
    gb_cache_start[0] = gb_base_addr;
    for(idx_set = 1; idx_set < SET_NUM; idx_set++)
    {
        gb_cache_start[idx_set] = gb_cache_start[idx_set - 1] + BLOCKSIZE/POINTER_LENGTH;
    }
    for(idx_hugepage_num = 0; idx_hugepage_num < 20; idx_hugepage_num++)
    {
        tmp_PFN = read_pagemap(path_buf, gb_base_addr + BLOCKSIZE/POINTER_LENGTH * (2048 * 64 /2) * idx_hugepage_num);
        printf("%d:%lld\n", idx_hugepage_num, tmp_PFN); //输出分配的hugepage的物理地址
    }
}


void init_cache_zone(int init_start)
{
    int idx_set, idx_assoc;
    gb_cache_zone[0][0] = gb_base_addr + init_start * CACHESPAN;
    for(idx_set = 1; idx_set < SET_NUM; idx_set++)
    {
        gb_cache_zone[idx_set][0] = gb_cache_zone[idx_set - 1][0] + BLOCKSIZE / POINTER_LENGTH;
    }
    for(idx_set = 0; idx_set < SET_NUM; idx_set++)
    {
        for(idx_assoc = 1; idx_assoc < ASSOCIATION * 3; idx_assoc++)
        {
            gb_cache_zone[idx_set][idx_assoc] = gb_cache_zone[idx_set][idx_assoc - 1] +  WAYSPAN;
        }
    }

}



void init_policy_pointer_chase()
{
    int idx_assoc;
    int cnt_set;
    unsigned long *ptr;
    for(cnt_set = 0; cnt_set < POLICY_NUM; cnt_set++)
    {
        gb_policy_low_first[cnt_set] = gb_cache_zone[PSEUDO_LOW_START + cnt_set][0];
    }
    for(cnt_set = 0; cnt_set < POLICY_NUM; cnt_set++)
    {
        ptr = gb_policy_low_first[cnt_set];
        for(idx_assoc = 1; idx_assoc < ASSOCIATION * 3; idx_assoc++)
        {
            *(void **)ptr = gb_cache_zone[PSEUDO_LOW_START + cnt_set][idx_assoc];
            ptr = gb_cache_zone[PSEUDO_LOW_START + cnt_set][idx_assoc];
        }
        *(void **)ptr = gb_policy_low_first[cnt_set];
    }
//	for(cnt_set = 0; cnt_set < POLICY_NUM; cnt_set++)
//	{
//		gb_policy_low_second[cnt_set] = gb_cache_zone[PSEUDO_LOW_START + cnt_set][0];
//	}
//	for(cnt_set = 0; cnt_set < POLICY_NUM; cnt_set++)
//	{
//		ptr = gb_policy_low_second[cnt_set];
//		for(idx_assoc = 1; idx_assoc < ASSOCIATION * 3; idx_assoc++)
//		{
//			*(void **)ptr = gb_cache_zone[PSEUDO_LOW_START + cnt_set][idx_assoc];
//			ptr = gb_cache_zone[PSEUDO_LOW_START + cnt_set][idx_assoc];
//		}
//		*(void **)ptr = gb_policy_low_second[cnt_set];
//	}
    for(cnt_set = 0; cnt_set < POLICY_NUM; cnt_set++)
    {
        gb_policy_high_first[cnt_set] = gb_cache_zone[PSEUDO_HIGH_START + cnt_set][0];
    }
    for(cnt_set = 0; cnt_set < POLICY_NUM; cnt_set++)
    {
        ptr = gb_policy_high_first[cnt_set];
        for(idx_assoc = 1; idx_assoc < ASSOCIATION * 3; idx_assoc++)
        {
            *(void **)ptr = gb_cache_zone[PSEUDO_HIGH_START + cnt_set][idx_assoc];
            ptr = gb_cache_zone[PSEUDO_HIGH_START + cnt_set][idx_assoc];
        }
        *(void **)ptr = gb_policy_high_first[cnt_set];
    }
//	for(cnt_set = 0; cnt_set < POLICY_NUM; cnt_set++)
//	{
//		gb_policy_high_second[cnt_set] = gb_cache_zone[PSEUDO_HIGH_START + cnt_set][0];
//	}
//	for(cnt_set = 0; cnt_set < POLICY_NUM; cnt_set++)
//	{
//		ptr = gb_policy_high_second[cnt_set];
//		for(idx_assoc = 1; idx_assoc < ASSOCIATION * 3; idx_assoc++)
//		{
//			*(void **)ptr = gb_cache_zone[PSEUDO_HIGH_START + cnt_set][idx_assoc];
//			ptr = gb_cache_zone[PSEUDO_HIGH_START + cnt_set][idx_assoc];
//		}
//		*(void **)ptr = gb_policy_high_second[cnt_set];
//	}
    //此处留着set20穿连用带宽挤压
    for(int i=0; i<20; i++)
    {
        ptr =  gb_cache_zone[i][0];
        for(idx_assoc = 1; idx_assoc < ASSOCIATION * 3; idx_assoc++)
        {
            *(void **)ptr = gb_cache_zone[i][idx_assoc];
            ptr = gb_cache_zone[i][idx_assoc];
        }
        *(void **)ptr =gb_cache_zone[i][0];
    }


}

//初始化bubble访问区
void init_bubble_zone()
{
    int i = 0;
    int idx_set, idx_assoc;
    //此处留着set20用带宽挤压
    for(idx_assoc = 0; idx_assoc < ASSOCIATION*3; idx_assoc++)
    {
        for(idx_set = 20; idx_set <= PSEUDO_LOW_START-1; idx_set++)
        {
            gb_bubble_zone[i++] = gb_cache_zone[idx_set][idx_assoc];
        }
//        printf("%d\n",i);
        for(idx_set = PSEUDO_LOW_END+1; idx_set <= PSEUDO_HIGH_START-1; idx_set++)
        {
            gb_bubble_zone[i++] = gb_cache_zone[idx_set][idx_assoc];
        }
        for(idx_set = PSEUDO_HIGH_END+1; idx_set < SET_NUM; idx_set++)
        {
            gb_bubble_zone[i++] = gb_cache_zone[idx_set][idx_assoc];
        }
    }
    //测试
    //printf("index of gb_bubble_zone is %d , length of gb_bubble_zone is %d\n", i, BUBBLE_ACCESS_LENGTH);
}
//访问专用组low的两个cache区域
void assign_low_policy(double delay)
{
    unsigned long *ptr;
    int idx_set, idx_assoc;
    //access low_first
    for(idx_set = 0; idx_set < POLICY_NUM; idx_set++)
    {
        ptr = gb_policy_low_first[idx_set];
        for(idx_assoc = 0; idx_assoc < ASSOCIATION * 2; idx_assoc++)
        {
            //printf("addr_policy_a is %#X\n", ptr);
            ptr = *(void **)ptr;
            double n=delay;
            while(n>0)
                n--;
        }
    }
}

//访问专用组high的两个区域
void assign_high_policy(double delay)
{
    unsigned long *ptr;
    int idx_set, idx_assoc;
    //access high_first
    for(idx_set = 0; idx_set < POLICY_NUM; idx_set++)
    {
        ptr = gb_policy_high_first[idx_set];
        for(idx_assoc = 0; idx_assoc < ASSOCIATION * 2; idx_assoc++)
        {
            //printf("attr_policy_b is %#X\n", ptr);
            ptr = *(void **)ptr;
            double n=delay;
            while(n>0)
                n--;
        }
    }
}
void assign_follow_policy(double delay)
{
    //follow set流了20个
    unsigned long *ptr;
    int idx_set, idx_assoc;
    for(idx_set = 0; idx_set < 20; idx_set++)
    {
        ptr =  gb_cache_zone[idx_set][0];
        for(idx_assoc = 0; idx_assoc < ASSOCIATION * 2; idx_assoc++)
        {
            //printf("attr_policy_b is %#X\n", ptr);
            ptr = *(void **)ptr;
            double n=delay;
            while(n>0)
                n--;
        }
    }
}

void memory_bubble_A(int bubble_local)
{
    //bubble_local 奇数需要带延时
    unsigned long *ptr[8];
    int idx_set, idx_assoc;
    if(bubble_local!=0)
    {
        int chase_num=(bubble_local+1)/2;
        for(int i=0; i<chase_num; i++)
        {
            ptr[i] = gb_policy_high_first[i];
        }
        for(idx_assoc = 0; idx_assoc < ASSOCIATION * 2; idx_assoc++)
        {
            for(int i=0; i<chase_num; i++)
            {
                ptr[i] = *(void **)ptr[i];
            }
            if(bubble_local%2==1)
            {
                double n=220/bubble_local/3;
                while(n>0)
                    n--;
            }
        }

    }
}
void memory_bubble_B(int bubble_local)
{
    //bubble_local 奇数需要带延时
    unsigned long *ptr[8];
    int idx_set, idx_assoc;
    if(bubble_local!=0)
    {
        int chase_num=(bubble_local+1)/2;
        for(int i=0; i<chase_num; i++)
        {
            ptr[i] = gb_policy_low_first[i];
        }
        for(idx_assoc = 0; idx_assoc < ASSOCIATION * 2; idx_assoc++)
        {
            for(int i=0; i<chase_num; i++)
            {
                ptr[i] = *(void **)ptr[i];
            }
            if(bubble_local%2==1)
            {
//                double n=70;
//                while(n>0)
//                    n--;
                double n=220/bubble_local/3;
                while(n>0)
                    n--;
            }
        }

    }
}

void memory_bubble_N(int bubble_local)
{
    //bubble_local 奇数需要带延时
    unsigned long *ptr[8];
    int idx_set, idx_assoc;
    if(bubble_local!=0)
    {
        int chase_num=(bubble_local+1)/2;
        for(int i=0; i<chase_num; i++)
        {
            ptr[i] = gb_cache_zone[i][0];
        }
        for(idx_assoc = 0; idx_assoc < ASSOCIATION * 2; idx_assoc++)
        {
            for(int i=0; i<chase_num; i++)
            {
                ptr[i] = *(void **)ptr[i];
            }
            if(bubble_local%2==1)
            {
//                double n=70;
//                while(n>0)
//                    n--;
                double n=220/bubble_local/3;
                while(n>0)
                    n--;
            }
        }

    }
}
