#ifndef RULES_H
#define RULES_H
#include "engine.h"

void set_hand(struct HandInfo *, TileRank, TileRank);
void rule_two_pair(void);
void house_way_build(void);

// ============================================================================
// IMPLEMENTATION
// ============================================================================
#ifdef RULES_IMPL

void set_hand(struct HandInfo *info, TileRank hi1, TileRank hi2) {
  TileRank lo1, lo2 = (TileRank)NO_TILE;
  size_t lo_count = 0;
  
  for (size_t i = 0; i < 4; i++) {
    if (info->tiles[i] != hi1 && info->tiles[i] != hi2) {
      if (lo_count == 0) lo1 = info->tiles[i];
      else lo2 = info->tiles[i];
      lo_count++;
    }
  }
  info->house_way = HAND_FROM_TILES(hi1, hi2, lo1, lo2);
}

void rule_two_pair(void) {
  for (uint16_t n = 0; n < NUM_LEGAL_PERMS; n++) {
    struct HandInfo *info = &HOUSE_WAY_MAP[n];
    
    if (info->num_pairs == 2) {

      // Two pairs: determine which pair is higher
      // Special case: Gee Jun pair is the highest pair despite low rank
      TileRank pair1 = info->tiles[0]; // tiles[0] == tiles[1]
      TileRank pair2 = info->tiles[2]; // tiles[2] == tiles[3]
      
      // If one pair is Gee Jun, it goes in high hand
      if (pair1 == RK_GEEJUN) {
        set_hand(info, info->tiles[0], info->tiles[1]);
      } else if (pair2 == RK_GEEJUN) {
        set_hand(info, info->tiles[2], info->tiles[3]);
      } else {
        // Otherwise, higher-ranked pair goes in high hand
        // Since hand is sorted descending, pair1 is always higher
        set_hand(info, info->tiles[0], info->tiles[1]);
      }
    }
  }
}

typedef void (*rule_fn_t)(void);
rule_fn_t HOUSE_WAY_RULES[] = {
  rule_two_pair,
  NULL // sentinel value
};

void house_way_build(void) {
  printf("Applying house way rules... \n");
  for (size_t i = 0; HOUSE_WAY_RULES[i] != NULL; i++) HOUSE_WAY_RULES[i]();
  printf("House way rules complete.\n");
}

#endif // RULES_IMPL
#endif // RULES_H
