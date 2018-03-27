#include <stdio.h>
#include "fake_mpi.h"

int MPI_Init(int *argc, char ***argv)
{
    printf("---(fake) MPI_Init: entered MPI_Init()+++\n");
    printf("(fake) MPI_Init: initializing stuff...\n");
    for (int i=0; i<*argc; ++i) {
        printf("argv[%d]: %s\n", i, (*argv)[i]);
    }

    printf("+++(fake) MPI_Init: ... done!+++\n");
    return 1;
}


int MPI_Finalize(void)
{
    printf("---(fake) MPI_Finalize: entered MPI_Finalize()+++\n");
    printf("(fake) MPI_Finalize: cleaning up stuff...\n");
    printf("+++(fake) MPI_Finalize: ... done!+++\n");
    return 1;
}


