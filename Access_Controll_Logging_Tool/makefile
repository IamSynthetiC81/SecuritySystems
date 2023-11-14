CC=gcc
CFLAGS =-Wall -Wextra -pedantic -g
LFLAGS =-lssl -lcrypto -ldl
DEBUG_FLAGS =-g -ggdb 
LIB_FLAGS =-fPIC -shared

OP_FLAGS = -D__HWRITE -D__HOPEN

all: compile_lib compile

compile: main.c
	@export LD_LIBRARY_PATH=./out; \
	export LD_PRELOAD=; \
	$(CC) $(CFLAGS) -o out/main main.c $(LFLAGS)

debug: main.c
	@export LD_LIBRARY_PATH=./out; \
	export LD_PRELOAD=; \
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o out/main main.c $(LFLAGS)

compile_lib: lib/*.c
	@export LD_PRELOAD=; \
	$(CC) $(LIB_FLAGS) $(FO_FLAGS) -o out/libmylib.so lib/log.c lib/fhandler.c lib/ACL.c $(OP_FLAGS) $(LFLAGS)

clean:
	rm -fr *.o *.so out/* 

test: test/fperm_test.c compile_lib
	./test/test_init.sh
	$(CC) $(CFLAGS) -o out/fperm_test test/fperm_test.c 
	@echo "Testing file permissions..."
	@LD_PRELOAD=./out/libmylib.so ./out/fperm_test