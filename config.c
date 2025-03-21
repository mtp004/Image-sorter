#include "main.h"
#include "config.h"

#define TYPE_BUFFER_SIZE 10

//external variable 
const char CONFIG_PATH[] = "/Users/tripham/Desktop/image-sorter/config.txt";
char trashPath[PATH_MAX];
char* typeBuffer[TYPE_BUFFER_SIZE] = {NULL};
int typeCount;

int LoadTrashPathConfig(const char line[BUFSIZ], char trashPath[], size_t trashPath_size){
    char* path = strchr(line, '=');
    if (!path) {
        printf("Error: trash path config not in the right format for reading\n");
        return -1;  // No '=' found in the line
    }
    path++;
    path = strtok(path, "\n ");
    if(path == NULL){
        printf("Error: trash path config uninitialized\n");
        return -1;
    }
    strncpy(trashPath, path, trashPath_size-1);
    trashPath[trashPath_size-1] = '\0';
    return 0;
}

int LoadFileTypeConfig(const char line[BUFSIZ], char* buffer[], int *count) {
    int len = 0;
    char* temp;

    *count = 0;
    char* tokenPointer = strchr(line, '=');
    if(tokenPointer){
        tokenPointer++;
        tokenPointer = strtok(tokenPointer, "\n ");

        if(tokenPointer == NULL){
            printf("Error: filetypes config uninitialized\n");
            return -1;
        }
        while(tokenPointer && *count<TYPE_BUFFER_SIZE){
            len = strlen(tokenPointer);

            if ((temp = (char*)malloc(len+1)) == NULL){
                FreeStaticMemory();
                printf("Error: failed to allocate memory\n");
                return -1;
            }
            buffer[*count] = temp;

            strncpy(buffer[*count], tokenPointer, len+1);
            tokenPointer = strtok(NULL, " ");
            (*count)++;
        }
    } else{
        printf("Error: config not in the right format for reading\n");
        return -1;
    }
    return 0;
}

int LoadConfigData(const char* configPath){
    char line[BUFSIZ];

    FILE *file = fopen(configPath, "r");
    if (!file) {
        perror("Error: Failed to open config file or file dont't exist\n");
        return -1;
    }

    if(fgets(line, BUFSIZ, file) == NULL){
        printf("Error: Failed to read filetypes config\n");
        return -1;
    }
    if(LoadFileTypeConfig(line, typeBuffer, &typeCount) == -1) return -1;

    if(fgets(line, BUFSIZ, file) == NULL){
        printf("Error: Failed to read trash bin path config\n");
        return -1;
    }
    if(LoadTrashPathConfig(line, trashPath, PATH_MAX) == -1) return -1;

    fclose(file);
    return 0;
}