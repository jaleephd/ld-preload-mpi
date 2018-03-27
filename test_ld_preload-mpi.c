#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

int MPI_Init(int *argc, char ***argv);
int MPI_Finalize(void);

/*
   overloading the following functions, to make sure they are
   only called once (from _init() and _fini()
    int MPI_Init(int *argc, char ***argv)
    int MPI_Finalize(void)
 */

typedef int (*mpi_init_fn)(int *argc, char ***argv);
typedef int (*mpi_fini_fn)(void);


/* code to get access to argv before main() is called. It relies on
   the fact that a unix standard is to push the argc and argv values
   right onto the top of the stack when a program starts.
   adapted from:
   http://blog.linuxgamepublishing.com/2009/10/12/argv-and-argc-and-just-how-to-get-them/
 */
char **get_argv()
{
  static char **preset_argv=NULL;

  if (!preset_argv) {
    extern char **environ; /* globally defined */ 
    char ***argvp;
    void *p;

    /* get top of stack (rsp is 64-bit SP) */
    asm volatile ("mov %%rsp, %0" : "=r" (p));
    argvp = p;

    /* read from top of stack until find the environ value */
    while (*argvp != environ) {
      argvp++;
    }

    /* then read back one value to get the pointer to argv */
    argvp--;
    preset_argv = *argvp;
  }

  return preset_argv;
}


void _init(void)
{
    int mpi_ret = 0;
    char **argv = get_argv();
    int argc = 0;
    while (argv[argc] != NULL) {
        ++argc;
    }

    printf("\n---_init---\n");
    printf("_init: __argc=%d, __argv[0]=%s\n",argc, argv[0]);
    printf("_init: calling MPI_init()...\n");
    mpi_ret = MPI_Init(&argc, &argv);
    printf("_init: MPI_Init returned with code %d!\n", mpi_ret);
    printf("_init: done!\n");
    printf("+++_init done!!!+++\n\n");
}


void _fini(void)
{
    int mpi_ret = 0;
    printf("\n---_fini---\n");
    printf("_fini: calling MPI_Finalize()...\n");
    mpi_ret = MPI_Finalize();
    printf("_fini: MPI_Finalize returned with code %d!\n", mpi_ret);
    printf("+++_fini done!!!+++\n\n");
}


int MPI_Init(int *argc, char ***argv)
{
    static int init_count;
    int ret_val = 0;
    init_count++;

    printf("\n---MPI_Init (OVERLOADED)---\n");
    printf("mpi_init entrance count %d...\n", init_count);

    mpi_init_fn real_MPI_Init = (mpi_init_fn)dlsym(RTLD_NEXT, "MPI_Init");

    printf("calling real MPI_Init() at address: %p...\n", (void*)real_MPI_Init);
    ret_val = real_MPI_Init(argc, argv);
    printf("+++MPI_Init (OVERLOADED) done!+++\n\n");

    return ret_val;
}


int MPI_Finalize(void)
{
    static int finalize_count;
    finalize_count++;
    printf("\n---MPI_Finalize (OVERLOADED)---\n");
    printf("mpi_finalize count %d...\n", finalize_count);

    mpi_fini_fn real_MPI_Finalize;
    real_MPI_Finalize = (mpi_fini_fn)dlsym(RTLD_NEXT, "MPI_Finalize");
    printf("calling real MPI_Finalize() at address: %p...\n", (void*)real_MPI_Finalize);
    real_MPI_Finalize();
    printf("+++MPI_Finalize (OVERLOADED) done!+++\n\n");

    return 0;
}



