#include <stdio.h>   // For input/output functions like fopen, fread, printf, etc.
#include <stdlib.h>  // For memory allocation, exit, etc.
#include <string.h>  // For string manipulation functions such as strcmp, strcpy, etc.
#include <dirent.h>  // For directory traversal functions like opendir, readdir, etc.
#include <sys/stat.h> // For file status functions like stat.
#include <sys/stat.h> // For getting image information

#pragma region Function Prototypes
char* GetDesktopPath(void);
DIR* OpenDirectory(char* path);
int IsImageFile(const char* filename);
void PrintImageInDirectory(DIR* dir);
#pragma endregion

int main(void) {
    printf("Sorting program starting...\n");

    char* homeDirectory = GetDesktopPath();
    DIR* sortDirectory = OpenDirectory(homeDirectory);
    PrintImageInDirectory(sortDirectory);
    free(homeDirectory);
    return 0;
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

void PrintImageInDirectory(DIR* dir) {
    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (
            entry->d_type == DT_REG 
            && IsImageFile(entry->d_name)
        ) { 
            printf("Found image: %s\n", entry->d_name);
        }
    }
}