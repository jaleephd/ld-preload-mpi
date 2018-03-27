#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>

typedef FILE* (*orig_fopen_fn_type)(const char*, const char*);

FILE* fopen(const char* pathname, const char* mode)
{
    printf("entered overloaded fopen()...\n");

    orig_fopen_fn_type real_fopen;
    real_fopen = (orig_fopen_fn_type)dlsym(RTLD_NEXT, "fopen");
    printf("calling fopen(%s, %s)...\n", pathname, mode);
    FILE* ret_val = real_fopen(pathname, mode);
    printf("done!\n");

    printf("leaving overloaded fopen()...\n");

    return ret_val;
}

