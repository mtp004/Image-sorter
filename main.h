#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include "config.h"

// Function Prototypes
int MoveDirectoryToTrash(const char* dpath);
int PromptDirectoryDeletion(char* wd, char* folderPath);
void FreeStaticMemory();
char* GetWorkingDirectoryPath(char* wd);
int IsImageFile(const char* filename);
void MoveFilesToFolder(char* wd, DIR* sourceDir, char* fpath);
char* GetSortedFolderPath(char* fname, char* wd);
void MoveFile(const char* sourcePath, const char* destinationPath);
int MakeFolder(const char* fpath);
int SortDirectory(char* dname);
int TokenizeAndProcess(char* str);
int DisplayUI();

#endif // MAIN_H
