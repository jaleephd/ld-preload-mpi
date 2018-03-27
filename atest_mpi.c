#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "fake_mpi.h"

int main(int argc, char **argv)
{
    int mpi_ret;
    printf("\n\n\n---main: entered main()---\n");

    printf("main: calling MPI_init()...\n");
    mpi_ret = MPI_Init(&argc, &argv);
    printf("main: MPI_Init returned with code %d!\n", mpi_ret);

    printf("main: doing something...\n");
    printf("main: ...done!\n");

    printf("main: calling MPI_Finalize()...\n");
    mpi_ret = MPI_Finalize();
    printf("main: MPI_Finalize returned with code %d!\n", mpi_ret);

    printf("\n+++main: leaving main()+++\n\n");
    return 0;
}

