#include <wait.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <semaphore.h>

int main()
{
	sem_t mutex;
	int fd, i;
	fd = open("./log.txt", O_RDWR|O_CREAT, S_IRWXU);
	sem_t *ptr = (sem_t*)mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if(sem_init(ptr, 1, 0) < 0)
		exit(0);
	for(i = 0; i < 2; i++)
		if(fork() == 0)
		{
			sem_wait(ptr);
			printf("child%d\n", i);
			exit(1);
		}
	sleep(5);
	printf("parent\n");
	sem_post(ptr);
//	sem_post(ptr);
	wait(0);
	printf("child finished\n");
	while(1);
	return 0;
}
