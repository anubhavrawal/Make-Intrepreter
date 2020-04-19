/**
 * C program to find file permission, size, creation and last modification date of 
 * a given file.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>


void printFileProperties(struct stat stats);


int main()
{
    char path[100] ="test/hworld.c";
    struct stat stats;

    // stat() returns 0 on successful operation,
    // otherwise returns -1 if unable to get file properties.
    if (stat(path, &stats) == 0)
    {
        printFileProperties(stats);
    }
    else
    {
        printf("Unable to get file properties.\n");
        printf("Please check whether '%s' file exists.\n", path);
    }

    return 0;
}



/**
 * Function to print file properties.
 */
void printFileProperties(struct stat stats){
    struct tm dt;

    // Get file creation time in seconds and 
    // convert seconds to date and time format
    dt = *(localtime(&stats.st_ctime));
    printf("\nCreated on: %d-%d-%d %d:%d:%d", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900, 
                                              dt.tm_hour, dt.tm_min, dt.tm_sec);

    // File modification time
    dt = *(localtime(&stats.st_mtime));
    printf("\nModified on: %d-%d-%d %d:%d:%d \n", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900, 
                                              dt.tm_hour, dt.tm_min, dt.tm_sec);

}