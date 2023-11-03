
all: Diffie_Hellman_Key_Exchange

Diffie_Hellman_Key_Exchange: DF_Key_exchange_threaded.c
	gcc -o dh_assign_1_threaded DF_Key_exchange_threaded.c -lgmp -lpthread -g -DVERBOSE -DDEBUG
	gcc -o dh_assign_1 DF_Key_exchange.c -lgmp -g -DVERBOSE