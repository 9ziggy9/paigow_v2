#include <stdio.h>

void hello_from_wasm(void) {
  printf("Hello from WASM!\n");
}

int main(void) {
  printf("WASM loaded!\n");
  return 0;
}
