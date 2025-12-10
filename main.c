#include <stdint.h>
#include <stdio.h>

void read_raw_tiles(uint16_t ts) {
  printf("Tiles from C: 0x%x\n", ts);
}

void wasm_ok(void) {
  printf("Successfully initialized cFFI and engine.\n");
}

int main(void) {
  printf("main.c is loaded\n");
  return 0;
}
