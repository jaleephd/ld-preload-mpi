#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
    printf("opening /etc/hostname...\n");
    FILE* fp = fopen("/etc/hostname", "r");
    if (!fp) {
        printf("error opening file!\n");
        return 0;
    }

    printf("opened file OK!\n");
    printf("closing /etc/hostname...\n");
    int retval = fclose(fp);
    if (retval) { // returns non-zero on fail
        printf("file close gave error: %d", errno);
    } else {
        printf("closed file OK!\n");
    }
    return 0;
}

