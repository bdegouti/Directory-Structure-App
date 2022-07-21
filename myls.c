#include "secret_headers.h"

const static int MAX_NUM_OF_PATHS = 1024;
const static int DIRECTORIES_SIZE = 64;
const static int DIRENT_SIZE = 64;
int pIndex = 0, longListing = 0, listSubRec = 0;



int main(int argc, char* argv[]){
    char* paths[MAX_NUM_OF_PATHS];
    int pathSize = 0;
    if(argc > 1){
        for(int i = 1; i < argc; i++){
            char* tmp = argv[i];
            if(tmp[0] == '-'){
                for(int k  = 1; k < strlen(tmp); k++){
                    if(tmp[k] == 'i' && pIndex == 0){
                        pIndex = 1;
                    }
                    else if(tmp[k] == 'l' && longListing == 0){
                        longListing = 1;
                    }
                    else if(tmp[k] == 'R' && listSubRec == 0){
                        listSubRec = 1;
                    }
                    else if(tmp[k] == ' '){
                        break;
                    }
                    else{
                        printf("Error: Unsupported Option\n");
                        exit(1);
                    }
                }
            }
            else{
                paths[pathSize] = argv[i];
                pathSize++;
            }
        }
    }
    runThroughPaths(paths, pathSize);
}

void printFileInfo(DIR* dir, char* path){
    size_t directoriesNum = 0, directoriesSize = DIRECTORIES_SIZE;
    struct dirent* directories = (struct dirent*) malloc(sizeof(struct dirent)*DIRECTORIES_SIZE);
    struct dirent* d;
    size_t fileNum = 0, filesSize = DIRENT_SIZE;
    struct dirent* fHolder = (struct dirent*) malloc(sizeof(struct dirent)*filesSize);
    struct stat fileInfo;

    int maxIno = 0, maxSize = 0, maxUser = 0, maxGroup = 0, maxLink = 0;

    while((d = readdir(dir)) != NULL){
        if(fileNum >= filesSize-1){
            fHolder = (struct  dirent*) realloc(fHolder, (filesSize*2));
            filesSize = filesSize*2;
        }
        
        if(d->d_ino > maxIno){
            maxIno = d->d_ino;
        }
        char loc[PATH_MAX];
        strcpy(loc, path);
        strcat(loc, "/");
        strcat(loc, d->d_name);
        if(!lstat(loc, &fileInfo)){
            if(fileInfo.st_size > maxSize){
                maxSize = fileInfo.st_size;
            }
            struct group *grp = getgrgid(fileInfo.st_gid);
            struct passwd *pw = getpwuid(fileInfo.st_uid);
            if(strlen(grp->gr_name) > maxGroup){
                maxGroup = strlen(grp->gr_name);
            }
            if(strlen(pw->pw_name) > maxUser){
                maxUser = strlen(pw->pw_name);
            }
            if(maxLink < fileInfo.st_nlink){
                maxLink = fileInfo.st_nlink;
            }
        }
        else{
            printf("Error: couldn't get file stats\n");
            exit(0);
        }
        fHolder[fileNum] = *d;
        fileNum++;
    }

    maxIno = (int)((ceil(log10(maxIno))+1)*sizeof(char));
    maxSize = (int)((ceil(log10(maxSize))+1)*sizeof(char));
    maxLink = (int)((ceil(log10(maxLink))+1)*sizeof(char));
    
    struct dirent tmp;
    for(int i = 0; i < fileNum; i++){
        for(int j= i+1; j < fileNum; j++){
            if(strcmp(fHolder[i].d_name, fHolder[j].d_name)>0){
                tmp = fHolder[i];
                fHolder[i] = fHolder[j];
                fHolder[j] = tmp;
            }
        }
    }

    for(int i = 0; i < fileNum; i++){
        char loc[PATH_MAX];
        strcpy(loc, path);
        strcat(loc, "/");
        strcat(loc, fHolder[i].d_name);
        if(fHolder[i].d_name[0] == '.'){
            continue;
        }
        if(pIndex){
            int currIno = (int)((ceil(log10(fHolder[i].d_ino))+1)*sizeof(char));
            for(int k = 0; k < maxIno-currIno; k++){
                printf(" ");
            }
            printf("%lu ", fHolder[i].d_ino);
        }
        if(listSubRec){
            if(!lstat(loc, &fileInfo)){
                if(S_ISDIR(fileInfo.st_mode)){
                    if(directoriesNum >= directoriesSize-1){
                        directories = (struct  dirent*) realloc(directories, (directoriesSize*2));
                        directoriesSize = directoriesSize*2;
                    }
                    directories[directoriesNum] = fHolder[i];
                    directoriesNum++; 
                }
            }
        }
        if(longListing){
            if(!lstat(loc, &fileInfo)){
                if(S_ISDIR(fileInfo.st_mode)){
                    printf("d");
                }
                else{
                    printf("-");
                }
                //user permissions
                if(fileInfo.st_mode & S_IRUSR){
                    printf("r");
                }
                else{
                    printf("-");
                }
                if(fileInfo.st_mode & S_IWUSR){
                    printf("w");
                }
                else{
                    printf("-");
                }
                if(fileInfo.st_mode & S_IXUSR){
                    printf("x");
                }
                else{
                    printf("-");
                }
                //group permissions
                if(fileInfo.st_mode & S_IRGRP){
                    printf("r");
                }
                else{
                    printf("-");
                }
                if(fileInfo.st_mode & S_IWGRP){
                    printf("w");
                }
                else{
                    printf("-");
                }
                if(fileInfo.st_mode & S_IXGRP){
                    printf("x");
                }
                else{
                    printf("-");
                }
                //others permissions
                if(fileInfo.st_mode & S_IROTH){
                    printf("r");
                }
                else{
                    printf("-");
                }
                if(fileInfo.st_mode & S_IWOTH){
                    printf("w");
                }
                else{
                    printf("-");
                }
                if(fileInfo.st_mode & S_IXOTH){
                    printf("x");
                }
                else{
                    printf("-");
                }
                char date[30];
                time_t t = fileInfo.st_mtim.tv_sec;
                strftime(date, 30, "%b %d %H:%M", localtime(&t));
                struct group *grp = getgrgid(fileInfo.st_gid);
                struct passwd *pw = getpwuid(fileInfo.st_uid);
                int currLink;

                if(fileInfo.st_nlink > 1){
                    currLink = (int)((ceil(log10(fileInfo.st_nlink))+1)*sizeof(char));
                }
                else{
                    currLink = 2;
                }
                
                for(int k = 0; k < maxLink-currLink; k++){
                    printf(" ");
                }
                printf(" %lu ", fileInfo.st_nlink);

                for(int k = 0; k < maxUser-strlen(pw->pw_name); k++){
                    printf(" ");
                }
                printf("%s ", grp->gr_name);
                for(int k = 0; k < maxUser-strlen(pw->pw_name); k++){
                    printf(" ");
                }
                printf("%s ", pw->pw_name);

                int currSize = (int)((ceil(log10(fileInfo.st_size))+1)*sizeof(char));
                for(int k = 0; k < maxSize-currSize; k++){
                    printf(" ");
                }
                printf("%ld ", fileInfo.st_size);
                printf("%s ", date);
                //printf(" %lu %s %s %ld %s ", fileInfo.st_nlink, pw->pw_name, grp->gr_name, fileInfo.st_size, date);
            }
            else{
                printf("Error: Couldn't get file stats\n");
                exit(0);
            }
        }
        printf("%s", fHolder[i].d_name);
        if(longListing){
            long unsigned int holdIno = fileInfo.st_ino;
            stat(loc, &fileInfo);
            if(fileInfo.st_ino != holdIno){
                char linkPath[PATH_MAX] = "";
                readlink(loc, linkPath, PATH_MAX);
                printf(" -> %s", linkPath);
            }
        }
        printf("\n");
    }
    if(listSubRec){
        for(int i = 0; i < directoriesNum; i++){
            char loc[PATH_MAX];
            strcpy(loc, path);
            strcat(loc, "/");
            strcat(loc, directories[i].d_name);
            printf("\n./%s\n", directories[i].d_name);
            closedir(dir);
            dir = opendir(loc);
            printFileInfo(dir, loc);
        }
    }
    free(directories);
    free(fHolder);
}


void runThroughPaths(char* paths[], int pathSize){
    DIR *dir;
    if(pathSize > 0){
        for(int i = 0; i < pathSize; i++){
            dir = opendir(paths[i]);
                if(!dir){
                    if(errno == ENOENT){
                        printf("Error: Nonexistant files or directories\n");
                        exit(1);
                    }
                    else{
                        struct stat fileInfo;
                        if(!lstat(paths[i], &fileInfo)){
                            if(pIndex){
                                printf("%lu ", fileInfo.st_ino);
                            }
                            if(longListing){
                                if(S_ISDIR(fileInfo.st_mode)){
                                    printf("d");
                                }
                                else{
                                    printf("-");
                                }
                                //user permissions
                                if(fileInfo.st_mode & S_IRUSR){
                                    printf("r");
                                }
                                else{
                                    printf("-");
                                }
                                if(fileInfo.st_mode & S_IWUSR){
                                    printf("w");
                                }
                                else{
                                    printf("-");
                                }
                                if(fileInfo.st_mode & S_IXUSR){
                                    printf("x");
                                }
                                else{
                                    printf("-");
                                }
                                //group permissions
                                if(fileInfo.st_mode & S_IRGRP){
                                    printf("r");
                                }
                                else{
                                    printf("-");
                                }
                                if(fileInfo.st_mode & S_IWGRP){
                                    printf("w");
                                }
                                else{
                                    printf("-");
                                }
                                if(fileInfo.st_mode & S_IXGRP){
                                    printf("x");
                                }
                                else{
                                    printf("-");
                                }
                                //others permissions
                                if(fileInfo.st_mode & S_IROTH){
                                    printf("r");
                                }
                                else{
                                    printf("-");
                                }
                                if(fileInfo.st_mode & S_IWOTH){
                                    printf("w");
                                }
                                else{
                                    printf("-");
                                }
                                if(fileInfo.st_mode & S_IXOTH){
                                    printf("x");
                                }
                                else{
                                    printf("-");
                                }
                                char date[30];
                                time_t t = fileInfo.st_mtim.tv_sec;
                                strftime(date, 30, "%b %d %H:%M", localtime(&t));
                                struct group *grp = getgrgid(fileInfo.st_gid);
                                struct passwd *pw = getpwuid(fileInfo.st_uid);
                                printf(" %lu %s %s %ld %s ", fileInfo.st_nlink, pw->pw_name, grp->gr_name, fileInfo.st_size, date);
                            }
                            printf("%s\n", paths[i]);
                            if(longListing){
                                int holdIno = fileInfo.st_ino;
                                stat(paths[i], &fileInfo);
                                if(fileInfo.st_ino != holdIno){
                                    char linkPath[PATH_MAX] = "";
                                    readlink(paths[i], linkPath, PATH_MAX);
                                    printf(" -> %s", linkPath);
                                }
                            }
                            exit(0);
                        }
                    }
                }
            printFileInfo(dir, paths[i]);
            if((pathSize-i) > 1){
                printf("\n");
            }
        }
    }
    else{
        char cwd[PATH_MAX];
        if(getcwd(cwd, sizeof(cwd)) == NULL){
            printf("Error: Couldn't find working directory.\n");
            exit(1);
        }
        dir = opendir(cwd);
        if(!dir){
            printf("Error: Cannot read directory\n");
            exit(1);
        }
        printFileInfo(dir, cwd);
    }
}