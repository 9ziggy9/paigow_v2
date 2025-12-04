CC=emcc
CFLAGS=-Wall -pedantic -Werror -Wconversion
EXPORTS=-sEXPORTED_FUNCTIONS=_hello_from_wasm,_main -sEXPORTED_RUNTIME_METHODS=ccall,cwrap

std.js: main.c
	$(CC) $(CFLAGS) $(EXPORTS) -o $@ $<


.PHONY: clean serve
serve:
	python -m http.server 8000

clean:	
	rm -rf std.js *.wasm
