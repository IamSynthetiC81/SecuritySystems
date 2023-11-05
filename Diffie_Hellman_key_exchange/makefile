all: dh_assign_1_threaded dh_assign_1 

dh_assign_1_threaded : DH_Key_exchange_threaded.c 
	gcc DH_Key_exchange_threaded.c -o dh_assign_1_threaded -g -lgmp -lpthread -DDEBUG -DVERBOSE

dh_assign_1 : DH_Key_exchange.c
	gcc DH_Key_exchange.c -o dh_assign_1 -g -lgmp -lpthread -DDEBUG -DVERBOSE

clean:
	rm -f dh_assign_1_threaded dh_assign_1
