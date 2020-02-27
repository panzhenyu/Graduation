#include <stdio.h>

int main(int argc, char *argv[])
{
	char *arg[] = {
	"1", "2", "3"
	};
	printf("%lu\n", sizeof(arg) / sizeof(char*));
	return 0;
}
