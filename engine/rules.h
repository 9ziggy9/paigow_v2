#ifndef RULES_H
#define RULES_H
#include "engine.h"

typedef struct {
  uint16_t points;
  size_t   num_variants;
  const TileRank variants[2];
} TileClass;

void set_hand(struct HandInfo *, TileRank, TileRank, TileRank, TileRank);
void rule_two_pair(void);
void rule_keep_pairs(void);
void house_way_build(void);
bool tile_matches_class(TileRank, const TileClass *);
bool tile_in_classes(TileRank, const TileClass* []);

#define TILE_CLASS(name, pts, n, ...) \
  const TileClass TC_##name = { pts, n, { __VA_ARGS__ } }

// Single tiles.
TILE_CLASS(GEEJUN,   3, 1, RK_GEEJUN);
TILE_CLASS(TEEN,     2, 1, RK_TEEN);
TILE_CLASS(DAY,      2, 1, RK_DAY);
TILE_CLASS(YUN,      8, 1, RK_YUN);
TILE_CLASS(GOR,      4, 1, RK_GOR);
TILE_CLASS(MUUY,     0, 1, RK_MUUY);
TILE_CLASS(CHONG,    6, 1, RK_CHONG);
TILE_CLASS(BON,      4, 1, RK_BON);
TILE_CLASS(FU,       1, 1, RK_FU);
TILE_CLASS(PING,     0, 1, RK_PING);
TILE_CLASS(CHIT,     7, 1, RK_CHIT);
TILE_CLASS(LOOK,     6, 1, RK_LOOK);
TILE_CLASS(CHOPGOW,  9, 1, RK_CHOPGOW);
TILE_CLASS(CHOPBOT,  8, 1, RK_CHOPBOT);
TILE_CLASS(CHOPCHIT, 7, 1, RK_CHOPCHIT);
TILE_CLASS(CHOPNG,   5, 1, RK_CHOPNG);

// Tile sets.
TILE_CLASS(TEEN_OR_DAY, 0, 2, RK_TEEN,  RK_DAY);
TILE_CLASS(ANY_TEN,     0, 2, RK_MUUY,  RK_PING);
TILE_CLASS(ANY_EIGHT,   8, 2, RK_YUN,   RK_CHOPBOT);
TILE_CLASS(ANY_SEVEN,   7, 2, RK_CHIT,  RK_CHOPCHIT);
TILE_CLASS(ANY_SIX,     6, 2, RK_CHONG, RK_LOOK);
TILE_CLASS(ANY_FOUR,    4, 2, RK_GOR,   RK_BON);

// ============================================================================
// IMPLEMENTATION
// ============================================================================
#ifdef RULES_IMPL

uint8_t rank_to_points(TileRank rk) {
  switch (rk) {
  case RK_GEEJUN:                  return 3;
  case RK_FU:                      return 1;
  case RK_CHOPGOW:                 return 9;
  case RK_CHOPNG:                  return 5;
  case RK_TEEN:  case RK_DAY:      return 2;
  case RK_YUN:   case RK_CHOPBOT:  return 8;
  case RK_GOR:   case RK_BON:      return 4;
  case RK_PING:  case RK_MUUY:     return 0;
  case RK_CHONG: case RK_LOOK:     return 6;
  case RK_CHIT:  case RK_CHOPCHIT: return 7;
  };
}

void set_hand(struct HandInfo *info,
              TileRank hi1, TileRank hi2,
              TileRank lo1, TileRank lo2)
{
  info->house_way = HAND_FROM_TILES(hi1, hi2, lo1, lo2);
}

void rule_two_pair(void) {
  printf("rule_two_pair()\n");
  for (uint16_t n = 0; n < NUM_LEGAL_PERMS; n++) {
    struct HandInfo *info = &HOUSE_WAY_MAP[n];
    
    if (info->num_pairs == 2) {
      TileRank p1 = info->tiles[0];
      TileRank p2 = info->tiles[2];
      if (p1 == RK_GEEJUN) set_hand(info, p1, p1, p2, p2);
      else if (p2 == RK_GEEJUN) set_hand(info, p2, p2, p1, p1);
      else set_hand(info, p1, p1, p2, p2);
    }
  }
}

// Helper: Check if a tile matches any variant in a TileClass
bool tile_matches_class(TileRank tile, const TileClass *tc) {
  for (size_t i = 0; i < tc->num_variants; i++) {
    if (tile == tc->variants[i]) return true;
  }
  return false;
}

// Helper: Check if tile matches any class in an array
bool tile_in_classes(TileRank tile, const TileClass *classes[]) {
  for (int i = 0; classes[i] != NULL; i++) {
    if (tile_matches_class(tile, classes[i])) return true;
  }
  return false;
}

void extract_non_paired(const struct HandInfo *info, TileRank *nps) {
  size_t other_idx = 0;
  for (size_t i = 0; i < 4; i++ ) {
    if (other_idx > 1) return;
    if (info->tiles[i] != info->paired_tile) nps[other_idx++] = info->tiles[i];
  }
}

void rule_keep_pairs(void) {
  for (uint16_t n = 0; n < NUM_LEGAL_PERMS; n++) {
    struct HandInfo *info = &HOUSE_WAY_MAP[n];
    if (info->num_pairs != 1 || info->is_splittable) continue;
    TileRank p = info->paired_tile;
    TileRank os[2] = { NO_TILE, NO_TILE };
    extract_non_paired(info, os);
    set_hand(info, p, p, os[0], os[1]);
  }
}

void rule_split_geejun(void) {
  for (uint16_t n = 0; n < NUM_LEGAL_PERMS; n++) {
    struct HandInfo *info = &HOUSE_WAY_MAP[n];
    if (info->paired_tile != RK_GEEJUN) continue;
    if (info->num_pairs != 1 || !info->is_splittable) continue;

    TileRank p = info->paired_tile;
    TileRank os[2] = { NO_TILE, NO_TILE };

    extract_non_paired(info, os);

    // We have to have at least ONE SIX
    if (!tile_matches_class(os[0], &TC_ANY_SIX) &&
        !tile_matches_class(os[1], &TC_ANY_SIX))
    {
      set_hand(info, p, p, os[0], os[1]);
      continue;
    }

    const TileClass *valid_snd[] = {
      &TC_ANY_FOUR, &TC_CHOPNG, &TC_ANY_SIX, NULL,
    };

    if (!tile_in_classes(os[0], valid_snd) ||
        !tile_in_classes(os[1], valid_snd)) continue;

    if (rank_to_points(os[0]) > rank_to_points(os[1]))
      set_hand(info, p, os[0], p, os[1]);
    else if (rank_to_points(os[0]) < rank_to_points(os[1]))
      set_hand(info, p, os[1], p, os[0]);
    else if (os[0] > os[1]) // tie break
      set_hand(info, p, os[0], p, os[1]);
    else
      set_hand(info, p, os[1], p, os[0]);
  }
}

typedef void (*rule_fn_t)(void);
rule_fn_t HOUSE_WAY_RULES[] = {
  rule_two_pair,
  rule_keep_pairs,
  rule_split_geejun,
  NULL // sentinel value
};

void house_way_build(void) {
  printf("Applying house way rules... \n");
  for (size_t i = 0; HOUSE_WAY_RULES[i] != NULL; i++) HOUSE_WAY_RULES[i]();
  printf("House way rules complete.\n");
}

#endif // RULES_IMPL
#endif // RULES_H
