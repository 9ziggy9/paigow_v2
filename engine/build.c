#include <stdlib.h>
#define ENGINE_IMPL
#define INFO_IMPL
#define RULES_IMPL

#include "engine.h"
#include "info.h"
#include "rules.h"

void export_house_way(const char *filename) {
  FILE *f = fopen(filename, "wb");
  if (!f) {
    fprintf(stderr, "Failed to open %s for writing\n", filename);
    exit(EXIT_FAILURE);
  }
  fwrite(HOUSE_WAY_MAP, sizeof(struct HandInfo), NUM_LEGAL_PERMS, f);
  fwrite(VALID_HAND_IDXS, sizeof(uint16_t), NUM_PERMS, f);
  fclose(f);
  printf("Exported house way to %s\n", filename);
}

int main(void) {
  house_way_init();
  house_way_analyze();
  house_way_build();
  export_house_way("engine.bin");
  return 0;
}
