CC=gcc
CFLAGS = -Wall -Wextra -pedantic 
LFLAGS = -lgmp -lpthread lib/libDH.a
DFLAGS = -DDEBUG -DVERBOSE -g 
LPATH = lib/
EPATH = etc/

all: libComp dh_assign_1_threaded dh_assign_1 


libComp : $(LPATH)DH.c
	$(CC) $(CFLAGS) -c $(LPATH)DH.c -o $(LPATH)DH.o
	$(CC) $(CFLAGS) -c $(LPATH)parser.c -o $(LPATH)parser.o

	ar rv $(LPATH)libDH.a $(LPATH)DH.o $(LPATH)parser.o
	ranlib $(LPATH)libDH.a

dh_assign_1 : DH_Key_exchange.c
	$(CC) DH_Key_exchange.c -o dh_assign_1 $(CFLAGS) $(LFLAGS) $(DFLAGS)

dh_assign_1_threaded : DH_Key_exchange_threaded.c 
	$(CC) DH_Key_exchange_threaded.c -o dh_assign_1_threaded $(CFLAGS) $(LFLAGS) $(DFLAGS)

clean:
	rm -vf dh_assign_1_threaded dh_assign_1 $(LPATH)libDH.a $(LPATH)DH.o $(EPATH)output.txt $(LPATH)parser.o 