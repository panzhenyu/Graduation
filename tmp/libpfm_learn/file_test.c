#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>

int main()
{
	FILE *fp = fopen("txt", "ro");
	char buff[1024];
	pid_t pid;
	pid = vfork();
	if(pid < 0)
		exit(1);
	else if(pid == 0)
	{
		fread(buff, 1, 1, fp);
		printf("%c\n", buff[0]);
		_exit(0);
	}
	else
	{
		fread(buff, 1, 1, fp);
		printf("%c\n", buff[0]);
	}
	return 0;
}
