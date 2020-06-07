#ifndef _JSONANALYSER_H
#define _JSONANALYSER_H

#include "cjson.h"

cJSON* openJsonFile(char *filename);
char* getCommandByName(cJSON *json, char *name);
char *getHomeByName(cJSON *json, char *name);
void cjsonFree(cJSON *json);

#endif