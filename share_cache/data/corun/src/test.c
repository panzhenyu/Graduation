#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

void f(int **k)
{
}

int main(int argc, char *argv[])
{
	int *a[2];
	f(a);
	return 0;
}
