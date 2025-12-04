CC=emcc
CFLAGS=-Wall -pedantic -Werror -Wconversion
EXPORTS=-sEXPORTED_FUNCTIONS=_hello_from_wasm,_main -sEXPORTED_RUNTIME_METHODS=ccall,cwrap

std.js: main.c
	$(CC) $(CFLAGS) $(EXPORTS) -o $@ $<

.PHONY: clean
clean:	
	rm -rf std.js *.wasm
