CC = gcc
CFLAGS = 

all: ftw1 ftw2 stree

ftw1: src/ftw1.c
	$(CC) $(CFLAGS) -o ftw1 src/ftw1.c

ftw2: src/ftw2.c
	$(CC) $(CFLAGS) -o ftw2 src/ftw2.c

stree: src/sTree.c
	$(CC) $(CFLAGS) -o stree src/sTree.c

clean:
	rm -f ftw1 ftw2
	rm -f stree