CC=emcc
CFLAGS=-Wall -pedantic -Werror -Wconversion

main: main.c
	$(CC) $(CFLAGS) -o std.js main.c

.PHONY: clean
clean:	
	rm -rf std.js *.wasm
