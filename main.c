#include <stdio.h>

void wasm_ok(void) {
  printf("Successfully initialized cFFI and engine.\n");
}

int main(void) {
  printf("main.c is loaded\n");
  return 0;
}
