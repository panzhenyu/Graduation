#include "include/jsonAnalyser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long getFileLength(FILE *fp)
{
	long length = 0;
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	rewind(fp);
	return length;
}

cJSON* openJsonFile(char *filename)
{
    FILE *fp;
	cJSON *json;
	char *data;
	long length;
    if((fp=fopen(filename, "r")) == NULL)
		return NULL;
	length = getFileLength(fp);
	data = (char*)malloc(sizeof(char) * (length + 1));
    if(data == NULL)
        return NULL;

	fread(data, sizeof(char), length, fp);
	data[length] = '\0';
    json = cJSON_Parse(data);
    free(data);
    fclose(fp);
    return json;
}

static char* collectCommand(cJSON *argArray)
{
    cJSON *arg;
    int num_ch = 1024, real_ch = 0, length;
    char *cmd = (char*)malloc(num_ch * sizeof(char));

    arg = cJSON_GetArrayItem(argArray, 0);
    while(arg)
    {
        length = strlen(arg->valuestring) + 1;
        while(real_ch + length >= num_ch)
        {
            num_ch *= 2;
            cmd = realloc(cmd, num_ch * sizeof(char));
        }
        strcpy(cmd+real_ch, arg->valuestring);
        real_ch += length - 1;
        strcpy(cmd+real_ch, " ");
        real_ch++;
        arg = arg->next;
    }
    cmd = realloc(cmd, (real_ch+1) * sizeof(char));         // +1 for \0 ch
    cmd[real_ch] = '\0';
    return cmd;
}

char* getCommandByName(cJSON *json, char *name)
{
    cJSON *arrayItem = cJSON_GetArrayItem(json, 0), *object;
    char *cmd;

    while(arrayItem)
    {
        object = cJSON_GetObjectItem(arrayItem, "name");
        if(strcmp(object->valuestring, name))
        {
            arrayItem = arrayItem->next;
            continue;
        }
        object = cJSON_GetObjectItem(arrayItem, "arguments");
        return collectCommand(object);
    }
    return NULL;
}

char* getHomeByName(cJSON *json, char *name)
{
    cJSON *arrayItem = cJSON_GetArrayItem(json, 0), *object;
    char *cmd;

    while(arrayItem)
    {
        object = cJSON_GetObjectItem(arrayItem, "name");
        if(strcmp(object->valuestring, name))
        {
            arrayItem = arrayItem->next;
            continue;
        }
        object = cJSON_GetObjectItem(arrayItem, "home");
        return strdup(object->valuestring);
    }
    return NULL;
}

void cjsonFree(cJSON *json)
{
    cJSON_Delete(json);
}
