#ifndef PAGEMAP_H
#define PAGEMAP_H

#include <stdint.h>
#include <stdio.h>

//读取pagemap和PFN操作的有关参数
#define PAGEMAP_ENTRY 8
#define GET_BIT(X,Y) (X & ((uint64_t)1<<Y)) >> Y
#define GET_PFN(X) X & 0x7FFFFFFFFFFFFF

static int __endian_bit = 1;
#define is_bigendian() ( (*(char*)&__endian_bit) == 0 )

int i, c, pid, status;
unsigned long virt_addr; 
uint64_t read_val, file_offset;
char path_buf [0x100];
FILE * f;
char *end;

unsigned long long read_pagemap(char * path_buf,unsigned long *temp);

int cache_mapping(unsigned long long n,int x);

#endif
