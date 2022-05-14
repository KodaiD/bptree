CC = g++
CFLAGS = -Wall -std=c++17 -g3
OBJS = bptree.o main.o

bptree.o: bptree.cpp
	$(CC) $(CFLAGS) -c bptree.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

run: $(OBJS)
	$(CC) $(CFLAGS) -o bptree $(OBJS)
	./bptree

clean: 
	rm -f *.o bptree
