CC = gcc -std=gnu99
COMPILER_WARNINGS = -Wall
GDB_FLAGS = -g
GCOV_FLAGS = -fprofile-arcs -ftest-coverage
GPROF_FLAGS = -pg -a
LD_LIBS = -lreadline -lcurses -lpthread
CFLAGS = $(COMPILER_WARNINGS) $(GDB_FLAGS) $(LD_LIBS)

all: userfs

userfs: userfs.c parse.c crash.c
	$(CC) $(CFLAGS) userfs.c  parse.c crash.c -o userfs

clean:
	/bin/rm -f userfs *.o *~
