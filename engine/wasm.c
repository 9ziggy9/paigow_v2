#include <emscripten.h>
#define ENGINE_IMPL
#define INFO_IMPL
#define RULES_IMPL

#include "engine.h"
#include "info.h"
#include "rules.h"

// Initialize the engine on WASM startup
EMSCRIPTEN_KEEPALIVE
void wasm_init(void) {
  printf("Initializing WASM engine...\n");
  house_way_init();
  house_way_analyze();
  house_way_build();
  printf("WASM engine ready!\n");
}

// Get house way for a given hand
EMSCRIPTEN_KEEPALIVE
uint16_t get_house_way(uint16_t hand) {
  struct HandInfo *info = get_hand_info(hand);
  if (info == NULL) return 0xFFFF;
  return info->house_way;
}
