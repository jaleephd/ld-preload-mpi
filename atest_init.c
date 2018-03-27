#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char **argv)
{
    printf("this is a main test!\n");
    for (int i=0; i<argc; ++i) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    return 0;
}

