#include "config.c"

#define TYPE_BUFFER_SIZE 10

int main(void) {
    printf("Program starting...\n");
    typeCount = 0;
    if(LoadConfigData(CONFIG_PATH) == -1) return EXIT_FAILURE;

    printf("Loaded file types:\n");
    for (int i = 0; i < typeCount; i++) {
        printf("%s\n", typeBuffer[i]);
    }
    printf("Trash bin path: %s\n", trashPath);

    while(DisplayUI() != -1);
    FreeStaticMemory();
    printf("Exited\n");
    return 0;
}

int DisplayUI() {
    printf("Enter HOME subdirectories separated by space | Press [ENTER] to exit program:\n");
    char buffer[BUFSIZ];
    if(fgets(buffer, BUFSIZ, stdin) == NULL){
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
    char buf[BUFSIZ];
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
    char srcFilePath[PATH_MAX];
    char dstFilePath[PATH_MAX];
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

int PromptDirectoryDeletion(char* wd, char* folderPath){
    printf("%s", wd);
    char buff[BUFSIZ];
    printf("Move folder to Trash? (y/n): ");
    fgets(buff, BUFSIZ, stdin);   

    if(buff[0] == 'y'){
        //Logic for deleting here
        MoveDirectoryToTrash(folderPath);
    } else if (buff[0] != 'n'){
        printf("Invalid input\n");
        return -1;
    }
    return 0;
}

int MoveDirectoryToTrash(const char* dpath){
    char buf[BUFSIZ];
    char timebuf[64];
    char final_path[PATH_MAX]; // Enough space for TRASH_PATH + des
    time_t now;
    struct tm *timeinfo;
    
    // Get current time
    time(&now);
    timeinfo = localtime(&now);
    
    // Format time in a more human-readable format
    strftime(timebuf, sizeof(timebuf), "_%b-%d-%Y_%I;%M%p", timeinfo);
    
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
    snprintf(final_path, sizeof(final_path), "%s/%s", trashPath, buf);

    if (rename(dpath, final_path) != 0) {
        // Handle error
        printf("Error moving directory: %s\n", strerror(errno));
        return -1;
    }
    printf("Success: %s ==> Trash\n", dpath);

    return 0;
}