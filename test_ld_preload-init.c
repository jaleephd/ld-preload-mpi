#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>


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
    char **argv = get_argv();
    int argc = 0;
    while (argv[argc] != NULL) {
        ++argc;
    }

    printf("_init: pre-main() setup __argc=%d, __argv[0]=%s\n",argc, argv[0]);
}


void _fini(void)
{
    printf("_fini: post-main() cleanup...\n");
}


