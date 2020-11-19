CC=gcc
CFLAGS=-Wall -g -std=c99 -D_DEFAULT_SOURCE 
LDFLAGS=-Iinclude -Llib -lminisat -lminisat-c -lrt

part1: 
	@echo "You need to run \"export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:lib\""
	$(CC) $(CFLAGS) graph_functions.c graph_colouring.c -o graph_colouring $(LDFLAGS)


clean:
	rm -f graph_colouring graph_colouring.o graph_functions.o 

.PHONY: part1 
