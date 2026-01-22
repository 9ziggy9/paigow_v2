#include <stdlib.h>
#define ENGINE_IMPL
#define UTILS_IMPL

#include "engine.h"
#include "utils.h"

void import_house_way(const char *filename) {
  FILE *f = fopen(filename, "rb");
  if (!f) {
    fprintf(stderr, "Failed to open %s for reading\n", filename);
    exit(EXIT_FAILURE);
  }
  fread(HOUSE_WAY_MAP, sizeof(struct HandInfo), NUM_LEGAL_PERMS, f);
  fread(VALID_HAND_IDXS, sizeof(uint16_t), NUM_PERMS, f);
  fclose(f);
  printf("Imported house way from %s\n", filename);
}

int main(void) {
  import_house_way("engine.bin");
  print_house_way_map(0, 100);
  return 0;
}
