#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>

void runThroughPaths(char* paths[], int pathSize);

void printFileInfo(DIR* dir, char* path);
