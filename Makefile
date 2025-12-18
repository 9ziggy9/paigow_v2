CC=emcc
CFLAGS=-Wall -pedantic -Werror -Wconversion
EXPORTS= -sEXPORTED_FUNCTIONS='[ \
"_house_way","_wasm_ok","_main"]'\
-sEXPORTED_RUNTIME_METHODS='["ccall","cwrap"]'

engine.js: main.c
	$(CC) $(CFLAGS) $(EXPORTS) -o $@ $<

.PHONY: clean serve
serve:
	python -m http.server 8000

clean:	
	rm -rf engine.js std.js *.wasm
