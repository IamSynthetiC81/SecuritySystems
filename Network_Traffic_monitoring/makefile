all: main

DIR_OUT = out
LOG_OUT = log

mkdir:
	mkdir -p $(DIR_OUT)/ $(LOG_OUT)/

main: mkdir pcap_ex.c
	gcc -o $(DIR_OUT)/pcap_ex pcap_ex.c -lpcap

clean: 
	rm -f $(DIR_OUT)/*

test: main
	@./main
