#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define ENGINE_IMPL
#include "engine.h"

typedef struct AnalysisPairs {
  bool has_pairs;
  bool has_gj;
  size_t gj_pos;
  size_t num_pairs;
  TileRank tiles[4];
  size_t pair_pos[2];
  uint16_t hand;
} AnalysisPairs;

void order_hand(AnalysisPairs *info, size_t p1, size_t p2) {
  info->hand = (uint16_t)
    HAND_FROM_TILES(info->tiles[p1],   info->tiles[p2],
                    info->tiles[p1^2], info->tiles[p2^2]);
}

// See XOR(2) property https://en.wikipedia.org/wiki/Exclusive_or
AnalysisPairs map_paired_hand(uint16_t hand) {
  AnalysisPairs info = {
    .has_pairs  = false,
    .tiles      = {0},
    .pair_pos   = {0},
    .gj_pos     = 0,
    .num_pairs  = 0,
    .hand       = 0,
  };

  UNPACK_HAND_INTO_BUFFER(info.tiles, hand);

  for (size_t n = 0; n < 3; n++) {
    if (info.tiles[n] == info.tiles[n + 1]) {
      if (info.tiles[n] == RK_GEEJUN) {
        info.has_gj = true;
        info.gj_pos = n;
      }
      info.has_pairs = true;
      info.pair_pos[info.num_pairs++] = n;
    }
  }

  if (info.has_pairs) {
    size_t p1 = info.pair_pos[0];
    size_t p2 = info.pair_pos[0] + 1;

    if (info.has_gj)
      order_hand(&info, info.gj_pos, info.gj_pos + 1);
    else if (info.num_pairs > 1 && info.tiles[p1] < info.tiles[p1 ^ 2])
      order_hand(&info, p1 ^ 2, p2 ^ 2);
    else
      order_hand(&info, p1, p2);

  }
  return info;
}

uint16_t house_way(uint16_t in) {
  printf("ENGINE: setting hand ... \n");

  uint16_t sorted = sort_hand_by_points(in);

  ExceptionMapping exception_filter = map_exception_hand(sorted);
  if (exception_filter.is_exception) return exception_filter.hand;

  AnalysisPairs pair_filter = map_paired_hand(sorted);
  if (pair_filter.has_pairs) return pair_filter.hand;

  return sorted;
}

void wasm_ok(void) {
  printf("ENGINE: successfully initialized cFFI and engine.\n");
}

int main(void) {
  printf("ENGINE: entering main ... \n");
  initialize_exceptions();
  return 0;
}
