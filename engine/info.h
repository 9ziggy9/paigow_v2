#ifndef INFO_H
#define INFO_H

#include "engine.h"

// Function declarations
uint8_t count_pairs(uint16_t h);
void info_has_geejun(void);
void info_count_pairs(void);
void house_way_analyze(void);

// ============================================================================
// IMPLEMENTATION
// ============================================================================

#ifdef INFO_IMPL

const bool is_splittable_pair[16] = {
  [RK_GEEJUN]   = true,  [RK_CHOPNG]   = false,
  [RK_CHOPCHIT] = true,  [RK_CHOPBOT]  = true,
  [RK_CHOPGOW]  = true,  [RK_LOOK]     = false,
  [RK_CHIT]     = true,  [RK_PING]     = false,
  [RK_FU]       = false, [RK_BON]      = false,
  [RK_CHONG]    = false, [RK_MUUY]     = false,
  [RK_GOR]      = false, [RK_YUN]      = true,
  [RK_DAY]      = true,  [RK_TEEN]     = true,
};

uint8_t count_pairs(uint16_t h) {
  uint8_t tiles[4];
  tiles[0] = GET_NTH_TILE(h, 0);
  tiles[1] = GET_NTH_TILE(h, 1);
  tiles[2] = GET_NTH_TILE(h, 2);
  tiles[3] = GET_NTH_TILE(h, 3);
  
  uint8_t pairs = 0;
  // check consecutive tiles for pairs
  // since hand is sorted, pairs will be adjacent
  if (tiles[0] == tiles[1]) pairs++;
  if (tiles[1] == tiles[2]) pairs++;
  if (tiles[2] == tiles[3]) pairs++;
  return pairs;
}

void info_tiles(void) {
  for (uint16_t n = 0; n < NUM_LEGAL_PERMS; n++) {
    struct HandInfo *info = &HOUSE_WAY_MAP[n];
    for (size_t i = 0; i < 4; i++) {
      if (info->tiles[i] == RK_GEEJUN) info->has_gj = true;
      if (info->tiles[i] == RK_TEEN || info->tiles[i] == RK_DAY)
        info->has_teen_or_day = true;
    }
  }
}

void info_count_pairs(void) {
  for (uint16_t n = 0; n < NUM_LEGAL_PERMS; n++) {
    struct HandInfo *info = &HOUSE_WAY_MAP[n];
    uint8_t num_pairs = count_pairs(info->house_way);
    info->num_pairs = num_pairs;
    if (num_pairs == 1) {
      TileRank paired_tile;

      if (info->tiles[0] == info->tiles[1]) paired_tile = info->tiles[0];
      else if (info->tiles[1] == info->tiles[2]) paired_tile = info->tiles[1];
      else paired_tile = info->tiles[2];

      info->paired_tile = paired_tile;
      if (is_splittable_pair[paired_tile]) info->is_splittable = true;
    }
  }
}

typedef void (*info_fn_t)(void);
info_fn_t HOUSE_WAY_INFO[] = {
  info_tiles,
  info_count_pairs,
  NULL // sentinel value
};

void house_way_analyze(void) {
  printf("Building house way strategy... \n");
  for (size_t i = 0; HOUSE_WAY_INFO[i] != NULL; i++) HOUSE_WAY_INFO[i]();
  printf("House way strategy complete.\n");
}

#endif // INFO_IMPL
#endif // INFO_H
