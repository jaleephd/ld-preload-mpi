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

#ifdef DEBUG
    printf("\n---_init---\n");
    printf("_init: __argc=%d, __argv[0]=%s\n",argc, argv[0]);
    printf("_init: calling MPI_init()...\n");
#endif

    mpi_ret = MPI_Init(&argc, &argv);

#ifdef DEBUG
    printf("_init: MPI_Init returned with code %d!\n", mpi_ret);
    printf("_init: done!\n");
    printf("+++_init done!!!+++\n\n");
#endif
}


void _fini(void)
{
    int mpi_ret = 0;
#ifdef DEBUG
    printf("\n---_fini---\n");
    printf("_fini: calling MPI_Finalize()...\n");
#endif

    mpi_ret = MPI_Finalize();

#ifdef DEBUG
    printf("_fini: MPI_Finalize returned with code %d!\n", mpi_ret);
    printf("+++_fini done!!!+++\n\n");
#endif
}


int MPI_Init(int *argc, char ***argv)
{
    static int init_count = 0;
    static int ret_val = 0;
    init_count++;

#ifdef DEBUG
    printf("\n---MPI_Init (OVERLOADED)---\n");
    printf("MPI_Init: mpi_init entrance count %d...\n", init_count);
#endif

    mpi_init_fn real_MPI_Init = (mpi_init_fn)dlsym(RTLD_NEXT, "MPI_Init");

    if (init_count==1) {
#ifdef DEBUG
        printf("MPI_Init: calling real MPI_Init() at address: %p...\n", (void*)real_MPI_Init);
#endif
        ret_val = real_MPI_Init(argc, argv);
    } else {
#ifdef DEBUG
        printf("MPI_Init: skipping repeat call to real MPI_Init()...\n");
#endif
    }

#ifdef DEBUG
    printf("+++MPI_Init (OVERLOADED) done!+++\n\n");
#endif

    return ret_val;
}


int MPI_Finalize(void)
{
    static int finalize_count = 0;
    finalize_count++;

#ifdef DEBUG
    printf("\n---MPI_Finalize (OVERLOADED)---\n");
    printf("MPI_Finalize: mpi_finalize count %d...\n", finalize_count);
#endif

    mpi_fini_fn real_MPI_Finalize;
    real_MPI_Finalize = (mpi_fini_fn)dlsym(RTLD_NEXT, "MPI_Finalize");

    if (finalize_count==1) {
#ifdef DEBUG
        printf("MPI_Finalize: calling real MPI_Finalize() at address: %p...\n", (void*)real_MPI_Finalize);
#endif
        real_MPI_Finalize();
    } else {
#ifdef DEBUG
        printf("MPI_Finalize: skipping repeat call to real MPI_Finalize()...\n");
#endif
    }

#ifdef DEBUG
    printf("+++MPI_Finalize (OVERLOADED) done!+++\n\n");
#endif

    return 0;
}



