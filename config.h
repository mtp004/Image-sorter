#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define TYPE_BUFFER_SIZE 10

extern const char CONFIG_PATH[];
extern char trashPath[PATH_MAX];
extern char* typeBuffer[TYPE_BUFFER_SIZE];
extern int typeCount;

// Function prototypes
int LoadConfigData(const char* configPath);
int LoadFileTypeConfig(const char line[BUFSIZ], char* buffer[], int *count);
int LoadTrashPathConfig(const char line[BUFSIZ], char trashPath[], size_t trashPath_size);

#endif
