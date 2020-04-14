#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
static time_t getFileCreationTime(char *path) {
    struct stat attr;
    stat(path, &attr);
    //printf("Last modified time: %s", ctime(&attr.st_mtime));
    return attr.st_mtime;
}
int main(){
    time_t t1;
    time_t t2;

    t1 = getFileCreationTime("test/hworld.c");
    t2 = getFileCreationTime("test/hworld.o");


    //printf("%s \nlength: %ld \nThe comparevalue is: %d \n", info1, strlen(info1), strncmp(info1, info2, 23));

    if (t1<t2){
        printf("Hello \n");
    }


	return 0;
}