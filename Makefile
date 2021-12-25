CC = cc
RM = rm

CFLAG = -O2 -Wall -Wextra -std=c99
obj = huffman.c list.h tree.h queue.h

.Phony: all

all:
	mkdir -p build
	$(CC) $(CFLAG) $(obj) -o build/huffman

test: 
	./build/huffman compress
	./build/huffman extract

clean:
	$(RM) -r ./build/*