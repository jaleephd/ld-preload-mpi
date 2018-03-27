CC = gcc
SHCC = ${CC} -fPIC -DPIC
LD = ld
CFLAGS = -Wall -std=gnu99
RELEASEFLAGS = -O2
DEBUGFLAGS = -g -DDEBUG
INCLUDEDIRS = -I.
LIBDIRS = -L.
DYNLINKLIB = -ldl
#SHOBJS = ${OBJS:.o=.so}

LIB_NAME = ld_preload_mpi
LIB_SRC = ${LIB_NAME}.c
REL_OBJS = ${LIB_NAME}.o
DBG_OBJS = ${LIB_NAME}_debug.o
REL_SHLIB = ${LIB_NAME}.so
DBG_SHLIB = ${LIB_NAME}_debug.so

# additional files for creating "test" target
TEST_NAME = atest_mpi
TEST_SRC = ${TEST_NAME}.c
TEST_OBJS = ${TEST_NAME}.o
TEST_EXE = ${TEST_NAME}
FAKE_MPI = fake_mpi
FAKE_MPI_SRC = ${FAKE_MPI}.c
FAKE_MPI_OBJS = ${FAKE_MPI}.o
FAKE_MPI_SHLIB = lib${FAKE_MPI}.so
TEST_LIB = test_ld_preload-mpi
TEST_LIB_SRC = ${TEST_LIB}.c
TEST_LIB_OBJS = ${TEST_LIB}.o
TEST_SHLIB = ${TEST_LIB}.so


.DEFAULT_GOAL := release


all: release debug test


# this generates the actual ld_preload_mpi shared library
release: ${REL_SHLIB}

${REL_SHLIB}: ${REL_OBJS}
	${LD} -shared -o ${REL_SHLIB} ${REL_OBJS} ${DYNLINKLIB}
	@echo "done!" 
	@echo "NOTE: pre-load the ${REL_SHLIB} shared library with LD_PRELOAD"
	@echo "" 

${REL_OBJS}: ${LIB_SRC}
	@echo "compiling release version of ${LIB_NAME} shared library with flags: ${RELEASEFLAGS} ..."
	${SHCC} ${CFLAGS} ${RELEASEFLAGS} ${INCLUDEDIRS} -c $< -o $@
#%.so: %.c
#	${SHCC} ${CFLAGS} ${RELEASEFLAGS} ${INCLUDEDIRS} -c $< -o $@
#%.o: %.c
#	@echo "compiling with release flags: ${RELEASEFLAGS} ..."
#	${SHCC} ${CFLAGS} ${RELEASEFLAGS} ${INCLUDEDIRS} -c $< -o $@


# the following is for adding debug information to ld_preload_mpi
debug: ${DBG_SHLIB}

${DBG_SHLIB}: ${DBG_OBJS}
	${LD} -shared -o ${DBG_SHLIB} ${DBG_OBJS} ${DYNLINKLIB}
	@echo "done!" 
	@echo "NOTE: pre-load the ${DBG_SHLIB} shared library with LD_PRELOAD"
	@echo "" 

${DBG_OBJS}: ${LIB_SRC}
	@echo "compiling debug version of ${LIB_NAME} shared library with debug flags: ${DEBUGFLAGS} ..."
	${SHCC} ${CFLAGS} ${DEBUGFLAGS} ${INCLUDEDIRS} -c $< -o $@


# the following creates test code and fake_mpi library
test: ${FAKE_MPI_SHLIB} ${TEST_SHLIB} ${TEST_EXE}

${FAKE_MPI_SHLIB}: ${FAKE_MPI_OBJS}
	${LD} -shared -o ${FAKE_MPI_SHLIB} ${FAKE_MPI_OBJS} ${DYNLINKLIB}
	@echo "done!" 
	@echo "" 

${FAKE_MPI_OBJS}: ${FAKE_MPI_SRC}
	@echo "compiling fake MPI library ..."
	${SHCC} ${CFLAGS} ${DEBUGFLAGS} ${INCLUDEDIRS} -c $< -o $@

${TEST_SHLIB}: ${TEST_LIB_OBJS}
	${LD} -shared -o ${TEST_SHLIB} ${TEST_LIB_OBJS} ${DYNLINKLIB}
	@echo "done!" 
	@echo "" 

${TEST_LIB_OBJS}: ${TEST_LIB_SRC}
	@echo "compiling test MPI library ..."
	${SHCC} ${CFLAGS} ${DEBUGFLAGS} ${INCLUDEDIRS} -c $< -o $@

${TEST_EXE}: ${TEST_SRC}
	@echo "compiling MPI test ..."
	${CC} ${CFLAGS} ${DEBUGFLAGS} ${INCLUDEDIRS} ${LIBDIRS} -l${FAKE_MPI} $< -o $@
	@echo "done!" 
	@echo "NOTE: before running ${TEST_EXE} :"
	@echo "      LD_LIBRARY_PATH must be set to directory containing ${TEST_SHLIB}" 
	@echo "      with, for example: export LD_LIBRARY_PATH=." 
	@echo "" 


options:
	@echo "Build options:"
	@echo "CC          = ${CC}"
	@echo "CCFLAGS     = ${CFLAGS}"
	@echo "SHCC        = ${SHCC}"
	@echo "INCLUDEDIRS = ${INCLUDEDIRS}"
	@echo "LIBDIRS     = ${LIBDIRS}"
	@echo "DYNLINKLIB  = ${DYNLINKLIB}"


help:
	@echo "Synopsis: make [ all | release | debug | test | options | help | clean ]"


clean:
	@echo "cleaning up..." 
	@rm -f ${REL_SHLIB} ${REL_OBJS} ${DBG_SHLIB} ${DBG_OBJS} ${FAKE_MPI_SHLIB} ${FAKE_MPI_OBJS} ${TEST_SHLIB} ${TEST_LIB_OBJS} ${TEST_EXE} *.so *.o core
	@echo "done!" 


.PHONY: all release debug test options help clean

