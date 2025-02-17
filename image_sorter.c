#include <stdio.h>   // For input/output functions like fopen, fread, printf, etc.
#include <stdlib.h>  // For memory allocation, exit, etc.
#include <string.h>  // For string manipulation functions such as strcmp, strcpy, etc.
#include <dirent.h>  // For directory traversal functions like opendir, readdir, etc.
#include <sys/stat.h> // For file status functions like stat.
#include <sys/stat.h> // For getting image information
#include <errno.h>

#pragma region Function Prototypes
char* GetDesktopPath(void);
DIR* OpenDirectory(char* path);
int IsImageFile(const char* filename);
void MoveFilesToFolder(DIR* sourceDir, char* fpath);
char* GetSortedFolderPath(char* fname);
void MoveFile(const char* sourcePath, const char* destinationPath);
void MakeFolder(const char* fpath);
#pragma endregion

//static variable
static char* homePath;
static char* folderPath;

int main(void) {
    printf("Sorting program starting...\n");

    homePath = GetDesktopPath();
    folderPath = GetSortedFolderPath("images");
    MakeFolder(folderPath);

    DIR* sortDirectory = OpenDirectory(homePath);
    MoveFilesToFolder(sortDirectory, folderPath);
    free(homePath);
    free(folderPath);
    return 0;
}

void MakeFolder(const char* fpath) {
    if (mkdir(fpath, 0777) == -1) {
        if (errno != EEXIST) {
            perror("Error creating folder");
            exit(1);
        } else {
            printf("Folder already exists: %s\n", fpath);
        }
    } else {
        printf("Created folder: %s\n", fpath);
    }
}

void MoveFilesToFolder(DIR* sourceDir, char* fpath) {
    struct dirent* entry;
    char srcFilePath[1024];
    char dstFilePath[1024];

    while ((entry = readdir(sourceDir)) != NULL) {
        if (
            entry->d_type == DT_REG 
            && IsImageFile(entry->d_name)
        ) { 
            printf("Found image: %s\n", entry->d_name);
            snprintf(srcFilePath, sizeof(srcFilePath), "%s/%s", homePath, entry->d_name);

            snprintf(dstFilePath, sizeof(dstFilePath), "%s/%s", fpath, entry->d_name);
            MoveFile(srcFilePath, dstFilePath);
        }
    }
}

void MoveFile(const char* sourcePath, const char* destinationPath) {
    if (rename(sourcePath, destinationPath) == 0) {
        printf("Moved: %s -> %s\n", sourcePath, destinationPath);
    } else {
        perror("Error moving file");
    }
}

char* GetSortedFolderPath(char* fname) {
    size_t pathLen = strlen(homePath) + strlen(fname) + 2;
    char* folderPath = (char*)malloc(pathLen);
    if (folderPath == NULL) {
        perror("Error allocating memory for sorted folder path");
        exit(1);
    }

    snprintf(folderPath, pathLen, "%s/%s", homePath, fname);
    return folderPath;
}

char* GetDesktopPath(void){
    char* homeDirectory = getenv("HOME");
    if(homeDirectory == NULL){
        printf("Error: failed to get HOME directory from environment variables");
        exit(1);
    }

    //creating buffer
    size_t pathLen = strlen(homeDirectory) + strlen("/Desktop");
    char* desktopPath = (char*)malloc(sizeof(char)*(pathLen+1));  //Plus one for null terminator
    if (desktopPath == NULL) {
        fprintf(stderr, "Error: memory allocation failed\n");
        exit(1);
    }
    // Construct the Desktop path
    snprintf(desktopPath, pathLen + 1, "%s/Desktop", homeDirectory);

    printf("Desktop path: %s\n", desktopPath);
    return desktopPath;
}

DIR* OpenDirectory(char* path){
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error: cannot open directory to sort");
        exit(1);
    }

    return dir;
}

int IsImageFile(const char* filename) {
    const char* extensions[] = {".jpg", ".png", ".jpeg", ".gif", ".bmp", ".tiff"};
    size_t numExtensions = sizeof(extensions) / sizeof(extensions[0]);

    for (size_t i = 0; i < numExtensions; i++) {
        if (strstr(filename, extensions[i])) { // Check if filename contains an image extension
            return 1;
        }
    }
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