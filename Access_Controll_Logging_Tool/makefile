CC=gcc -std=c11
CFLAGS =-Wall -Wextra -pedantic 
LFLAGS = -llog -lfhandler -lssl -lcrypto -ldl 
DEBUG_FLAGS =-g -ggdb 
LIB_FLAGS =-fPIC -shared
OP_FLAGS = -D__HWRITE -D__HOPEN

LIB_ARGS= -Ilib -L$(DIR_OUT_LIB) -Wl,-rpath=./$(DIR_OUT_LIB)

DIR_OUT=out/
DIR_INP_TEST = test/
DIR_OUT_TEST = out/test/
DIR_INP_LIB = lib/
DIR_OUT_LIB = out/lib/

# debug = -DDEBUG
debug = 

PRELOAD_ON = LD_PRELOAD=./$(DIR_OUT_LIB)libACL.so
PRELOAD_OFF = LD_PRELOAD=

LIB_INCLUDE =LD_LIBRARY_PATH=./$(DIR_OUT_LIB)

all: compile_lib main acmonitor aclog

create_dirs:
	@mkdir -p $(DIR_OUT_TEST)
	@mkdir -p $(DIR_OUT_LIB)

main: compile_lib main.c
	$(CC) $(CFLAGS) -o out/main main.c -Ilib -L$(DIR_OUT_LIB) -Wl,-rpath=./$(DIR_OUT_LIB) $(LFLAGS)

debug: compile_lib main.c
	@export LD_LIBRARY_PATH=$(DIR_OUT_LIB); \
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o out/main main.c $(LFLAGS)
acmonitor: create_dirs compile_lib acmonitor.c
	$(CC) $(CFLAGS) -o $(DIR_OUT)acmonitor acmonitor.c -Ilib -L$(DIR_OUT_LIB) -Wl,-rpath=./$(DIR_OUT_LIB) $(LFLAGS)
aclog : test_aclog.c
	$(CC) $(CFLAGS) -o $(DIR_OUT)aclog test_aclog.c -Ilib -L$(DIR_OUT_LIB) -Wl,-rpath=./$(DIR_OUT_LIB) $(LFLAGS)

compile_lib: create_dirs $(DIR_INP_LIB)/*.c
	@echo "Compiling library files ..."

	gcc -shared -fPIC -o $(DIR_OUT_LIB)libMisc.so lib/Misc.h -Ilib -g $(debug)
	gcc -shared -fPIC -o $(DIR_OUT_LIB)libfhandler.so lib/fhandler.c -Ilib -ldl -g
	gcc -shared -fPIC -o $(DIR_OUT_LIB)liblog.so lib/log.c $(LIB_ARGS) -g $(debug) 
	gcc -shared -fPIC -o out/lib/libACL.so lib/ACL.c -Ilib -Lout/lib/ -lfhandler -Wl,-rpath=./out/lib/ $(LFLAGS) -D__HWRITE -D__HOPEN -g 
	
	@#gcc -shared -fPIC -o $(DIR_OUT_LIB)libACL.so lib/ACL.c -Ilib -Lout/lib -lfhandler $(OP_FLAGS)
clean:
	rm -fr *.o *.so out/* test/.testfiles

test: compile_lib test/*.c
	@./test/test_init.sh
	
	@echo "Compiling test files ..."
	$(CC) $(CFLAGS) -o $(DIR_OUT_TEST)fperm_test $(DIR_INP_TEST)fperm_test.c -Ilib -L$(DIR_OUT_LIB) -Wl,-rpath=./$(DIR_OUT_LIB) $(LFLAGS) $(debug) -g
	$(CC) $(CFLAGS) -o $(DIR_OUT_TEST)fmod_test $(DIR_INP_TEST)fmod_test.c -Ilib -L$(DIR_OUT_LIB) -Wl,-rpath=./$(DIR_OUT_LIB) $(LFLAGS) $(debug) -g
	$(CC) $(CFLAGS) -o $(DIR_OUT_TEST)sarray_test $(DIR_INP_TEST)sarray_test.c -Ilib -L$(DIR_OUT_LIB) -Wl,-rpath=./$(DIR_OUT_LIB) $(LFLAGS) $(debug) -g
	
	@echo "Testing String Array ..."
	@$(PRELOAD_ON) ./$(DIR_OUT_TEST)sarray_test
	@echo "Testing file permissions ..."
	@$(PRELOAD_ON) ./$(DIR_OUT_TEST)fperm_test
	@echo "Testing Modification Monitoring ..."
	@$(PRELOAD_ON) ./$(DIR_OUT_TEST)fmod_test  $(debug)