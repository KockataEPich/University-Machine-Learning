CFLAGS=-g -Wall -std=c99
LDFLAGS=-lm
CC=gcc

# "make darray", "make bstree" or "make hash" to compile your code

darray: 
	$(CC) $(CFLAGS) -DDARRAY $(LDFLAGS) global.c speller.c darray.c sorting.c -o speller_darray 

bstree:
	$(CC) $(CFLAGS) -DBSTREE $(LDFLAGS) global.c speller.c bstree.c -o speller_bstree

hash: 
	$(CC) $(CFLAGS) -DHASH $(LDFLAGS) global.c speller.c hashset.c -o speller_hash


tidy:
	-rm *.o speller_darray speller_bstree speller_hash 

.PHONY: darray bstree hash

################################################################################
# Comment out the next line once you have debugged your code
#RUN=valgrind
# Uncomment the next line to time your code (instead of using valgrind)
RUN=time
# If you comment out both RUNs above you can run your program by itself

# to test: e.g. "make testdarray"

MODE=-m 4

testdarray: darray
	$(RUN) ./speller_darray -d data/sample-dictionary -vvv $(MODE) data/sample-file

# small dictionaries that give trees that are unbalanced in various ways
testtree: bstree
	#$(RUN) ./speller_bstree -d data/LLdictionary $(MODE) -vvv data/sample-file-tree
	#$(RUN) ./speller_bstree -d data/LRdictionary $(MODE) -vvv data/sample-file-tree
	#$(RUN) ./speller_bstree -d data/RLdictionary $(MODE) -vvv data/sample-file-tree
	#$(RUN) ./speller_bstree -d data/RRdictionary $(MODE) -vvv data/sample-file-tree
	@echo
	$(RUN) ./speller_bstree -d data/sample-file $(MODE) -vv data/sample-file

testhash: hash
	$(RUN) ./speller_hash -d data/sample-dictionary -s 13 $(MODE) -vv data/sample-file
	@echo "This mightd fail as the dictionary is bigger than the hashtable"
	$(RUN) ./speller_hash -d data/sample-dictionary -s 5 $(MODE) -vvv data/sample-file

