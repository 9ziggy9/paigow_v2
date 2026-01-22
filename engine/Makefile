CC = gcc
CFLAGS = -Wall -Wextra -O2

all: paigow

# Main program (uses engine.bin)
paigow: main.c engine.h utils.h
	$(CC) $(CFLAGS) main.c -o paigow

# Generator program (creates engine.bin)
build: build.c engine.h info.h rules.h
	$(CC) $(CFLAGS) build.c -o build

# Generate the engine.bin file
engine.bin: build
	./build

clean:
	rm -f paigow build engine.bin

run: paigow engine.bin
	./paigow

# Full rebuild
rebuild: clean engine.bin paigow

.PHONY: all clean run rebuild
