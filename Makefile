CC = g++
CFLAGS = -Wall -std=c++17 -g3
OBJS = bptree.o internal_node.o leaf_node.o main.o

bptree.o: bptree.cpp
	$(CC) $(CFLAGS) -c bptree.cpp

internal_node.o: internal_node.cpp
	$(CC) $(CFLAGS) -c internal_node.cpp

leaf_node.o: leaf_node.cpp
	$(CC) $(CFLAGS) -c leaf_node.cpp

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

run: $(OBJS)
	$(CC) $(CFLAGS) -o bptree $(OBJS)
	./bptree

clean: 
	rm -f $(OBJS) bptree
