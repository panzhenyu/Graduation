#ifndef REDIRECT_H
#define REDIRECT_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "exp_data.h"


//const char *file_name = "benchmark_feature_data_v1.txt";

void redirect_to_file(const char *filename, const char *path, benchmark_data_list_t *data_list);

void serialize_to_file(const char* filenam, const char* path, profile_data_list_t* list);

#endif
