CC = emcc
CFLAGS = -O3 -s WASM=1 -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]'
ENGINE_DIR = engine

all: engine.js

# Compile WASM from engine code
engine.js: $(ENGINE_DIR)/wasm.c $(ENGINE_DIR)/engine.h $(ENGINE_DIR)/info.h $(ENGINE_DIR)/rules.h
	$(CC) $(CFLAGS) \
		-s EXPORTED_FUNCTIONS='["_wasm_init","_get_house_way","_malloc","_free"]' \
		$(ENGINE_DIR)/wasm.c \
		-o engine.js

clean:
	rm -f engine.js engine.wasm

# Native engine builds (for testing/development)
native:
	$(MAKE) -C $(ENGINE_DIR)

.PHONY: all clean native
