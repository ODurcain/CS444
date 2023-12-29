CC=gcc
CFLAGS=-Wall -O2

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

Compile:

	gcc HuffmanEncoding.c -o HuffmanEncoding

Run:

	./HuffmanEncoding -i completeshakespeare.txt -o HuffmanEncoding.out

Compare:

	diff HuffmanEncoding.out <filename>.out (replace <filename> with the file name of the file being compared)

clean:
	rm -f *.out