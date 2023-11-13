CC=gcc
CFLAGS = -Wall -Wextra -pedantic -g
LFLAGS =  -lssl -lcrypto -ldl
FO_FLAGS = -D__OVER_WRITE -D__OVER_OPEN
DEBUG_FLAGS = -g -ggdb 
LIB_FLAGS = -fPIC -shared


all: compile_lib set_lib compile

set_lib: 
	export LD_LIBRARY_PATH=./out

compile: main.c
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o out/main main.c -L./out -lmylib -I./lib $(LFLAGS)

compile_lib: lib/*.c
	$(CC) $(LIB_FLAGS) $(FO_FLAGS) $(LFLAGS) -o out/libmylib.so lib/log.c lib/fhandler.c lib/ACL.c 

clean:
	rm -f test.txt *.o *.so out/*.*