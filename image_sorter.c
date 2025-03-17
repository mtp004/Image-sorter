#include <stdio.h>   // For input/output functions like fopen, fread, printf, etc.
#include <stdlib.h>  // For memory allocation, exit, etc.
#include <string.h>  // For string manipulation functions such as strcmp, strcpy, etc.
#include <dirent.h>  // For directory traversal functions like opendir, readdir, etc.
#include <sys/stat.h> // For getting image information
#include <errno.h>
#include <unistd.h>
#include <time.h>

#pragma region Function Prototypes
int MoveDirectoryToTrash(const char* dpath);
int PromptDirectoryDeletion(char* wd, char* folderPath);
void FreeStaticMemory();
char* GetWorkingDirectoryPath(char* wd);
int IsImageFile(const char* filename);
void MoveFilesToFolder(char* wd,DIR* sourceDir, char* fpath);
char* GetSortedFolderPath(char* fname, char* wd);
void MoveFile(const char* sourcePath, const char* destinationPath);
int MakeFolder(const char* fpath);
int SortDirectory(char* dname);
int TokenizeAndProcess(char* str);
int DisplayUI();
int LoadFileTypeConfig(const char *filename, char* buffer[], int *count);
#pragma endregion

#define CONFIG_PATH "/Users/tripham/Desktop/image-sorter/config.txt"
#define TRASH_PATH "/Users/tripham/.Trash"
#define TYPE_BUFFER_SIZE 10

//static variable
static char* typeBuffer[TYPE_BUFFER_SIZE] = {NULL};
static int typeCount;


int main(void) {
    printf("Program starting...\n");
    typeCount = 0;
    if(LoadFileTypeConfig(CONFIG_PATH, typeBuffer, &typeCount) == -1) return EXIT_FAILURE;

    printf("Loaded file types:\n");
    for (int i = 0; i < typeCount; i++) {
        printf("%s\n", typeBuffer[i]);
    }

    while(DisplayUI() != -1);
    FreeStaticMemory();
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
    for(int i = 0; i<typeCount; i++){
        free(typeBuffer[i]);
        typeBuffer[i] = NULL;
    }
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
    printf("\n- - - - - - - - - - - - - - - - -");
    char* wd = GetWorkingDirectoryPath(dname);  //No check
    char* sortedPath = GetSortedFolderPath("images", wd);   //No check
    if(MakeFolder(sortedPath) == -1){
        printf("Error: Entered directory '%s' does not exist\n", dname);
        free(wd);
        free(sortedPath);
        return -1;
    }

    DIR* sortDirectory = opendir(wd);
    MoveFilesToFolder(wd ,sortDirectory, sortedPath);  //no check
    closedir(sortDirectory);

    while(PromptDirectoryDeletion(wd, sortedPath) != 0);

    free(wd);
    free(sortedPath);
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

void MoveFilesToFolder(char* wd,DIR* sourceDir, char* sortPath) {
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

            snprintf(dstFilePath, sizeof(dstFilePath), "%s/%s", sortPath, entry->d_name);
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

    printf("\n%s path: %s\n", wd, wdpath);
    return wdpath;
}

int IsImageFile(const char* filename) {
    // size_t numExtensions = sizeof(filetypes) / sizeof(filetypes[0]);
    for (size_t i = 0; i < (size_t)typeCount; i++) {
        if (strstr(filename, typeBuffer[i])) { // Check if filename contains an image extension
            return 1;
        }
    }
    return 0;
}

int LoadFileTypeConfig(const char *filename, char* buffer[], int *count) {
    int len = 0;
    char* temp;

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open config file");
        return -1;
    }
    *count = 0;
    char line[256];
    if(fgets(line, 256, file) != NULL){
        char* tokenPointer = strchr(line, '=');
        if(tokenPointer){
            tokenPointer++;
            tokenPointer = strtok(tokenPointer, " ");
            while(tokenPointer && *count<TYPE_BUFFER_SIZE){
                len = strlen(tokenPointer);

                if ((temp = (char*)malloc(len+1)) == NULL){
                    FreeStaticMemory();
                    printf("Error: failed to allocate memory");
                    return -1;
                }
                buffer[*count] = temp;

                strncpy(buffer[*count], tokenPointer, len+1);
                tokenPointer = strtok(NULL, " ");
                (*count)++;
            }
        }
    } else return -1;

    fclose(file);
    return 0;
}

int PromptDirectoryDeletion(char* wd, char* folderPath){
    printf("%s", wd);
    char buff[100];
    printf("Move folder to Trash? (y/n): ");
    fgets(buff, 100, stdin);   

    if(buff[0] == 'y'){
        //Logic for deleting here
        MoveDirectoryToTrash(folderPath);
        printf("Deleted everything\n");
    } else if (buff[0] != 'n'){
        printf("Invalid input\n");
        return -1;
    }
    return 0;
}

int MoveDirectoryToTrash(const char* dpath){
    char buf[256];
    char timebuf[64];
    char final_path[512]; // Enough space for TRASH_PATH + des
    time_t now;
    struct tm *timeinfo;
    
    // Get current time
    time(&now);
    timeinfo = localtime(&now);
    
    // Format time in a more human-readable format
    strftime(timebuf, sizeof(timebuf), "_%b-%d-%Y_%I:%M%p", timeinfo);
    
    // Original path processing
    char* slash = strrchr(dpath, '/');
    slash--;
    while(*slash != '/') slash--;
    slash++;
    strcpy(buf, slash);
    
    // Replace slash with underscore
    slash = strchr(buf, '/');
    if(slash){
        *slash = '_';
    }
    
    // Create the destination filename
    size_t remaining_space = sizeof(buf) - strlen(buf) - 1;
    strncat(buf, timebuf, remaining_space);
    
    // Combine TRASH_PATH with the destination filename
    snprintf(final_path, sizeof(final_path), "%s/%s", TRASH_PATH, buf);

    if (rename(dpath, final_path) != 0) {
        // Handle error
        printf("Error moving directory: %s\n", strerror(errno));
        return -1;
    }
    printf("Folder successfully moved to Trash\n");

    return 0;
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

