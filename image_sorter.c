#include <stdio.h>   // For input/output functions like fopen, fread, printf, etc.
#include <stdlib.h>  // For memory allocation, exit, etc.
#include <string.h>  // For string manipulation functions such as strcmp, strcpy, etc.
#include <dirent.h>  // For directory traversal functions like opendir, readdir, etc.
#include <sys/stat.h> // For file status functions like stat.
#include <sys/stat.h> // For getting image information
#include <errno.h>

#pragma region Function Prototypes
void FreeStaticMemory();
char* GetWorkingDirectoryPath(char* wd);
DIR* OpenDirectory(char* path);
int IsImageFile(const char* filename);
void MoveFilesToFolder(char* wd,DIR* sourceDir, char* fpath);
char* GetSortedFolderPath(char* fname, char* wd);
void MoveFile(const char* sourcePath, const char* destinationPath);
int MakeFolder(const char* fpath);
int SortDirectory(char* dname);
int TokenizeAndProcess(char* str);
int DisplayUI();
char **LoadFileTypeConfig(const char *filename, int *count);
#pragma endregion

#define CONFIG_PATH "/Users/tripham/Desktop/image-sorter/config.txt"
#define MAX_TYPES 10

//static variable
static char* sortedPath;
static char** filetypes;
static int typeCount;

int main(void) {
    printf("Program starting...\n");
    typeCount = 0;
    filetypes = LoadFileTypeConfig(CONFIG_PATH, &typeCount);

    printf("Loaded file types:\n");
    for (int i = 0; i < typeCount; i++) {
        printf("%s\n", filetypes[i]);
    }

    while(DisplayUI() != -1);
    printf("Exited\n");
    return 0;
}

int DisplayUI() {
    printf("Enter HOME subdirectories separated by space | Press [ENTER] to exit program:\n");
    char buffer[200];
    if(fgets(buffer, 200, stdin) == NULL){
        perror("Input not properly received");
        return -1;
    }
    size_t len = strlen(buffer);
    buffer[len - 1] = '\0';

    if(buffer[0] == '\0') return -1;

    if(TokenizeAndProcess(buffer) == 0){
        printf("SUCCESS\n");
    }
    return 0;
}

void FreeStaticMemory(){
    free(sortedPath);
    sortedPath = NULL;
}

int TokenizeAndProcess(char* str) {
    int count = 0;
    char buf[100];
    //Loop through the string
    while (*str) {
        // Skip spaces
        while (*str == ' ') str++;   
        // Count characters
        count = 0;
        while (*str && *str != ' ') {
            count++;
            str++;
        }
        if (count > 0){
            strncpy(buf, str-count, count);
            buf[count]='\0';
            if(SortDirectory(buf) == -1) return -1;
        } else return -1;
    }
    return 0;
}

int SortDirectory(char* dname){
    char* wd = GetWorkingDirectoryPath(dname);  //No check
    sortedPath = GetSortedFolderPath("images", wd);   //No check
    if(MakeFolder(sortedPath) == -1){
        printf("Error: Entered directory '%s' does not exist\n", dname);
        FreeStaticMemory(); 
        return -1;
    }

    DIR* sortDirectory = opendir(wd);
    MoveFilesToFolder(wd ,sortDirectory, sortedPath);  //no check
    closedir(sortDirectory);
    FreeStaticMemory();
    return 0;
}

int MakeFolder(const char* fpath) {
    if (mkdir(fpath, 0777) == -1) {
        if (errno != EEXIST) {
            return -1;
        } else {
            printf("Folder already exists: %s\n", fpath);
        }
    } else {
        printf("Created folder: %s\n", fpath);
    }
    return 0;
}

void MoveFilesToFolder(char* wd,DIR* sourceDir, char* fpath) {
    struct dirent* entry;
    char srcFilePath[1024];
    char dstFilePath[1024];
    while ((entry = readdir(sourceDir)) != NULL) {
        //printf("(%d, %d) ", entry->d_type, IsImageFile(entry->d_name));
        if (
            entry->d_type == DT_REG 
            && IsImageFile(entry->d_name)
        ) { 
            printf("  Found image: %s\n", entry->d_name);
            snprintf(srcFilePath, sizeof(srcFilePath), "%s/%s", wd, entry->d_name);

            snprintf(dstFilePath, sizeof(dstFilePath), "%s/%s", fpath, entry->d_name);
            MoveFile(srcFilePath, dstFilePath);
        }
    }
}

void MoveFile(const char* sourcePath, const char* destinationPath) {
    if (rename(sourcePath, destinationPath) == 0) {
        printf("    Moved -> %s\n", destinationPath);
    } else {
        perror("Error moving file");
    }
}

char* GetSortedFolderPath(char* fname, char* wd) {
    size_t pathLen = strlen(wd) + strlen(fname) + 2;
    char* folderPath = (char*)malloc(pathLen);
    if (folderPath == NULL) {
        perror("Error allocating memory for sorted folder path");
        FreeStaticMemory();
        exit(1);
    }

    snprintf(folderPath, pathLen, "%s/%s", wd, fname);
    return folderPath;
}

char* GetWorkingDirectoryPath(char* wd){
    char* homeDirectory = getenv("HOME");
    if(homeDirectory == NULL){
        printf("Error: failed to get HOME directory from environment variables");
        exit(1);
    }

    //creating buffer
    size_t pathLen = strlen(homeDirectory) + strlen(wd) + 2; //Plus 2 for "/" and null terminator
    char* wdpath = (char*)malloc(sizeof(char)*(pathLen));  //Plus 1 for null terminator
    if (wdpath == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(1);
    }
    // Construct the Desktop path
    snprintf(wdpath, pathLen, "%s/%s", homeDirectory, wd);

    printf("%s path: %s\n", wd, wdpath);
    return wdpath;
}

int IsImageFile(const char* filename) {
    // size_t numExtensions = sizeof(filetypes) / sizeof(filetypes[0]);
    for (size_t i = 0; i < (size_t)typeCount; i++) {
        if (strstr(filename, filetypes[i])) { // Check if filename contains an image extension
            return 1;
        }
    }
    return 0;
}

char **LoadFileTypeConfig(const char *filename, int *count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open config file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    char **types = NULL;
    *count = 0;  // Reset count

    if (fgets(line, sizeof(line), file)) {
        char *start = strchr(line, '=');
        if (start) {
            start++;  // Move past '='

            char *token = strtok(start, ", \n");  // Split by comma, space, newline
            while (token && *count < MAX_TYPES) {
                types = realloc(types, (*count + 1) * sizeof(char *));
                if (!types) {
                    perror("Memory allocation failed");
                    exit(EXIT_FAILURE);
                }

                types[*count] = malloc(strlen(token) + 1);
                if (!types[*count]) {
                    perror("Memory allocation failed");
                    exit(EXIT_FAILURE);
                }

                strcpy(types[*count], token);
                (*count)++;

                token = strtok(NULL, ", \n");
            }
        }
    }

    fclose(file);
    return types;
}

// void PrintImageInDirectory(DIR* dir) {
//     struct dirent* entry;
//     while ((entry = readdir(dir)) != NULL) {
//         if (
//             entry->d_type == DT_REG 
//             && IsImageFile(entry->d_name)
//         ) { 
//             printf("Found image: %s\n", entry->d_name);
//         }
//     }
// }