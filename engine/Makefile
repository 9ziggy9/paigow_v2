CC=gcc
CFLAGS=-Wall -pedantic -Werror -Wconversion

main: main.c
	$(CC) $(CFLAGS) main.c -o run

build: build.c
	$(CC) $(CFLAGS) build.c -o build
	./build && rm build

clean:
	rm -rf run build engine.bin
