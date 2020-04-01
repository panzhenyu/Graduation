#include "pagemap.h"

//读pagemap,返回PFN（一个页的物理基地址）
unsigned long long read_pagemap(char * path_buf,unsigned long *temp)
{
	unsigned long virt_addr = (unsigned long)temp;

	f = fopen(path_buf, "rb");
	if(!f)
	{
		printf("Error! Cannot open %s\n", path_buf);
		return -1;
	}

	//Shifting by virt-addr-offset number of bytes
	//and multiplying by the size of an address (the size of an entry in pagemap file)

	file_offset = virt_addr / getpagesize() * PAGEMAP_ENTRY;

	status = fseek(f, file_offset, SEEK_SET);
	if(status)
	{
		perror("Failed to do fseek!");
		return -1;
	}

	read_val = 0;
	unsigned char c_buf[PAGEMAP_ENTRY];
	for(i=0; i < PAGEMAP_ENTRY; i++)
	{
		c = getc(f);
	
		if(c == EOF)
		{
			printf("\nReached end of the file\n");
			return 0;
		}

		if(is_bigendian())
		{
			c_buf[i] = c;
		}
		else
		{
			c_buf[PAGEMAP_ENTRY - i - 1] = c;
		}
	}

	for(i=0; i < PAGEMAP_ENTRY; i++)
	{
		read_val = (read_val << 8) + c_buf[i];
	}

	if(GET_BIT(read_val, 63))
	{
		fclose(f);
		return ((unsigned long long) GET_PFN(read_val));
	}
	else
	{
		return -1;
	}

	fclose(f);
}

//16进制转化为2进制
int cache_mapping(unsigned long long n,int x)
{
	int bit[32];
	int i;
	
	int low_bit, high_bit;

	n = n *16 *16 *16;
	n = n + (x % 64) * 64; 
	for (i = 0; i < 32; i++)
	{
		bit[i] = n % 2;
		n /= 2;
	}

	/*******************************************************/
	//不同的机器的异或位是不同的,不用的机器需要进行修改
	/*******************************************************/
	low_bit = bit[30] ^ bit[28] ^ bit[27] ^ bit[26] ^ bit[25] ^ bit[24] ^ bit[22] ^ bit[20] ^ bit[18] ^ bit[17] ^ bit[16] ^ bit[14] ^ bit[12] ^ bit[10] ^ bit[6]  ;
	high_bit = bit[31] ^ bit[29] ^ bit[28] ^ bit[26] ^ bit[24] ^ bit[23] ^ bit[22] ^ bit[21] ^ bit[20] ^ bit[19] ^ bit[17] ^ bit[15] ^ bit[13] ^ bit[11] ^ bit[7]  ;

	return high_bit * 2 + low_bit;
}
