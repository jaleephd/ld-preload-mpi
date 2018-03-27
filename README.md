# ld-preload-mpi

A shared library and example code demonstrating the use of `ld_preload`, `MPI_init()` and `MPI_fini()`
for hooking into file system accesses in jobs submitted to a Linux-based HPC cluster.


## ld_preload_mpi Shared Library

The `ld_preload_mpi` shared library code calls `MPI_Init()` prior to `main()`
being called, and `MPI_Finalize()` after `main()` finishes. It also overrides
`MPI_Init(int *argc, char ***argv)` and `MPI_Finalize()` function calls,
ensuring that they are only called ONCE during a program's execution.
The code has embedded debugging printf's that can be enabled by
compiling with `-DDEBUG`.

### Files

* library code is in `ld_preload_mpi.c`
* test with `fake_mpi.{h,c}` and `atest_mpi.c` (see example 3)

### Manual Compiling

To compile the `ld_preload` library code
```
gcc -g -Wall -fPIC -DPIC -c ld_preload_mpi.c
ld -shared -o ld_preload_mpi.so ld_preload_mpi.o -ldl
```
And to compile with debugging outputs
```
gcc -g -Wall -fPIC -DPIC -DDEBUG -c ld_preload_mpi.c
ld -shared -o ld_preload_mpi_debug.so ld_preload_mpi.o -ldl
```

### Run a Program Within Wrapper

Run program wrapped in `ld_preload_mpi`, with
```
LD_PRELOAD=</path/to>/ld_preload_mpi.so <program_name>
```

## Compiling Library and Examples

This directory contains test, debug and working code for `ld_preload`
and for wrapping all programs within MPI initialise and finalise with
`_init()` and `_fini()`

To compile, use `make` with one of the following targets:
* release (default): generates the actual `ld_preload_mpi` shared library
* debug: generates the `ld_preload_mpi` shared library with debugging outputs
         (for debugging only!)
* test: generates the `atest_mpi` executable, `libfake_mpi.so` shared library
        (for testing without MPI installed), and `test_ld_preload-mpi`
        (test library). See Example 3 below for details.
* all: generates release, debug and test targets
* options: shows build options
* help: shows make targets (synopsis)
* clean: removes all make-generated files


## Example 1

The simplest example uses `LD_PRELOAD` to override the `fopen()` system call.

### References

For more information see the following references:
* http://www.catonmat.net/blog/simple-ld-preload-tutorial/
* http://rafalcieslak.wordpress.com/2013/04/02/dynamic-linker-tricks-using-ld_preload-to-cheat-inject-features-and-investigate-programs/
* http://www.linuxjournal.com/article/7795

### Files

* `atest_fopen.c`
* `test_ld_preload-fopen.c`

### Compiling

Compile shared library with:
```
gcc -Wall -fPIC -shared -ldl -o test_ld_preload-fopen.so test_ld_preload-fopen.c
```

This creates a `test_ld_preload-fopen.so` shared library.
Note: need `-ldl` to include the `dlsym()` function from the `libdl` library.

Compile a test `main()` program:
```
gcc -std=gnu99 atest_fopen.c -o atest_fopen
```

### Run Example

Run example with:
```
LD_PRELOAD=./test_ld_preload-fopen.so ./atest_fopen
```


## Example 2

The second example executes initilisation code prior to main being called
and shows how to get `main()`'s argc and argv parameters from the stack; and
executes cleanup code after main() completes.

### References

For more information see the following references:
* _init: http://hackerboss.com/overriding-system-functions-for-fun-and-profit/
* get_argv based on: http://blog.linuxgamepublishing.com/2009/10/12/argv-and-argc-and-just-how-to-get-them/

### Files
* `atest_init.c`
* `test_ld_preload-init.c`

### Compiling

Compile test `main()` program:D with
```
gcc -std=gnu99 atest_init.c -o atest_init
```

Compile shared library with
```
gcc -g -Wall -fPIC -DPIC -c test_ld_preload-init.c
ld -shared -o test_ld_preload-init.so test_ld_preload-init.o -ldl
```

Run with
```
LD_PRELOAD=./test_ld_preload-init.so ./atest_init ninety nine
```

## Example 3

The third example overrides `MPI_Init(int *argc, char ***argv)` and
`MPI_Finalize()` function calls, as well as calling `MPI_Init()` prior to
`main()` being called, and `MPI_Finalize()` after `main()` finishes.

References: see above.

### Files

* `atest_mpi.c`
* `fake_mpi.h`
* `fake_mpi.c`
* `test_ld_preload-mpi.c`

### Compiling

Compile `fake_mpi` library with
```
gcc -g -std=gnu99 -Wall -fPIC -DPIC -c fake_mpi.c
ld -shared -o libfake_mpi.so fake_mpi.o -ldl
```

This creates a `libfake_mpi.so` shared library.
Note needs to be `lib<name>.so` for `-l<name>` to find the library.

To compile atest_mpi.c using shared library `libfake_mpi.so` in current dir
```
gcc -L. -lfake_mpi -g -std=gnu99 atest_mpi.c -o atest_mpi
```

Compile preload test shared library code with
```
gcc -g -Wall -fPIC -DPIC -c test_ld_preload-mpi.c
ld -shared -o test_ld_preload-mpi.so test_ld_preload-mpi.o -ldl
```

Set library path to current directory for linker to find `fake_mpi` library,
and run test with initialise, finalise and overloaded functions, with
```
export LD_LIBRARY_PATH=.
LD_PRELOAD=./test_ld_preload-mpi.so ./atest_mpi ninety nine
```

