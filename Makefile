CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic
LFLAGS = -lm

.PHONY: all clean

all: encode decode

encode: code.o huffman.o io.o node.o pq.o stack.o encode.o
	$(CC) $(CFLAGS) code.o huffman.o io.o node.o pq.o stack.o encode.o -o encode

decode: code.o huffman.o io.o node.o pq.o stack.o decode.o
	$(CC) $(CFLAGS) code.o huffman.o io.o node.o pq.o stack.o decode.o -o decode

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf encode decode *.o

scan-build: clean
	scan-build make 
