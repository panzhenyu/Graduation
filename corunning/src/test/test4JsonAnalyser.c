#include "../include/jsonAnalyser.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "../include/process.h"
#define DEBUG

int main(int argc, char *argv[])
{
    cJSON *benchmarkConfig = openJsonFile("../../config/benchmark.json");
    char *home, *command, **env;
    int num;
    char * const envp[] = {NULL};
    home = getHomeByName(benchmarkConfig, argv[1]);
    command = getCommandByName(benchmarkConfig, argv[1]);
    env = cmd2env(command, &num);
    printf("%s\n", home);
    printf("%s\n", command);
    chdir(home);
    execve(env[0], env, envp);
    return 0;
}