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
void extract_unmatched(const TileRank t,
                       const struct HandInfo *info,
                       TileRank *ums)
{
  size_t other_idx = 0;
  for (size_t i = 0; i < 4; i++) {
    if (other_idx > 2) return;
    if (info->tiles[i] != t) ums[other_idx++] = info->tiles[i];
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

void set_pair_from_class_list(struct HandInfo *info,
                         const TileRank  *os,
                         const TileClass *cls[])
{
  TileRank p = info->paired_tile;
  if (!(tile_in_classes(os[0], cls) && tile_in_classes(os[1], cls)))
    return set_hand(info, p, p, os[0], os[1]);
  if (rank_to_points(os[0]) > rank_to_points(os[1]))
    return set_hand(info, p, os[0], p, os[1]);
  if (rank_to_points(os[0]) < rank_to_points(os[1]))
    return set_hand(info, p, os[1], p, os[0]);
  if (os[0] > os[1]) // tie break by rank
    return set_hand(info, p, os[0], p, os[1]);
  return set_hand(info, p, os[1], p, os[0]);
}

void aux_split_geejun(struct HandInfo *info, const TileRank *os) {
  if (!tile_matches_class(os[0], &TC_ANY_SIX) &&
      !tile_matches_class(os[1], &TC_ANY_SIX))
    return set_hand(info, info->paired_tile, info->paired_tile, os[0], os[1]);
  return set_pair_from_class_list(info, os, (const TileClass *[]) {
      &TC_ANY_FOUR, &TC_CHOPNG, &TC_ANY_SIX, NULL
  });
}

void aux_split_teen_day(struct HandInfo *info, const TileRank *os) {
  TileRank p = info->paired_tile;
  // For exactly, 9/11
  if ((tile_matches_class(os[0], &TC_FU) &&
       tile_matches_class(os[1], &TC_CHOPGOW)) ||
      (tile_matches_class(os[1], &TC_FU) &&
       tile_matches_class(os[0], &TC_CHOPGOW)))
  {
    if (rank_to_points(os[0]) > rank_to_points(os[1]))
      set_hand(info, p, os[0], p, os[1]);
    else
      set_hand(info, p, os[1], p, os[0]);
    return;
  }
  // we temporarily use a value of 12 to denote teen/day points
  uint8_t sum_h1 = rank_to_points(os[0]) + 12;
  uint8_t sum_h2 = rank_to_points(os[1]) + 12;

  // TODO: DOUBLE CHECK THIS!!!!
  // for a bare minimum split, we would have the case that we have precisely
  // a four and a 6; this implies a total sum of 34
  if (sum_h1 + sum_h2 < 34) return;
  if (sum_h1 > sum_h2)      return set_hand(info, p, os[0], p, os[1]);
  if (sum_h2 > sum_h1)      return set_hand(info, p, os[1], p, os[0]);
  if (os[0] > os[1])        return set_hand(info, p, os[0], p, os[1]);
                            return set_hand(info, p, os[1], p, os[0]);
}

void aux_split_nines(struct HandInfo *info, const TileRank *os) {
  set_pair_from_class_list(info, os, (const TileClass *[]){
      &TC_TEEN_OR_DAY, &TC_ANY_TEN, NULL
  });
}

void aux_split_eights(struct HandInfo *info, const TileRank *os) {
  if ((tile_matches_class(os[0], &TC_FU) &&
       tile_matches_class(os[1], &TC_CHOPGOW)) ||
      (tile_matches_class(os[1], &TC_FU) &&
       tile_matches_class(os[0], &TC_CHOPGOW)))
  {
    if (rank_to_points(os[0]) < rank_to_points(os[1]))
      return set_hand(info, info->paired_tile, os[0], info->paired_tile, os[1]);
    return set_hand(info, info->paired_tile, os[1], info->paired_tile, os[0]);
  }
  return set_pair_from_class_list(info, os, (const TileClass *[]){
      &TC_TEEN_OR_DAY, &TC_ANY_TEN, &TC_FU, NULL
  });
}

void aux_split_sevens(struct HandInfo *info, const TileRank *os) {
  if (!info->has_teen_or_day)
    return set_hand(info, info->paired_tile, info->paired_tile, os[0], os[1]);
  return set_pair_from_class_list(info, os, (const TileClass *[]){
      &TC_TEEN_OR_DAY, &TC_ANY_TEN, &TC_FU, NULL
  });
}

void rule_split_pair(void) {
  for (uint16_t n = 0; n < NUM_LEGAL_PERMS; n++) {
    struct HandInfo *info = &HOUSE_WAY_MAP[n];
    if (info->num_pairs != 1 || !info->is_splittable) continue;

    TileRank os[2] = { NO_TILE, NO_TILE };
    extract_non_paired(info, os);

    switch (info->paired_tile) {
      case RK_GEEJUN:
        aux_split_geejun(info, os); break;
      case RK_TEEN : case RK_DAY:
        aux_split_teen_day(info, os); break;
      case RK_CHOPGOW:
        aux_split_nines(info, os); break;
      case RK_YUN: case RK_CHOPBOT:
        aux_split_eights(info, os); break;
      case RK_CHIT: case RK_CHOPCHIT:
        aux_split_sevens(info, os); break;
      default: break;
    }
  }
}

bool at_least_high_3(TileRank t1, TileRank t2) {
  uint8_t points = (rank_to_points(t1) + rank_to_points(t2)) % 10;
  if (points > 3) return true;
  if (points == 3) return (t1 > RK_CHONG) || (t2 > RK_CHONG);
  return false;

}

void rule_wong_gong_nine(void) {
  for (uint16_t n = 0; n < NUM_LEGAL_PERMS; n++) {
    struct HandInfo *info = &HOUSE_WAY_MAP[n];
    if (!info->has_teen_or_day || info->num_pairs) continue;
    size_t teen_day_idx = 0;
    for (size_t i = 0; i < 4; i++) {
      if (tile_matches_class(info->tiles[i], &TC_TEEN_OR_DAY)) {
        teen_day_idx = i;
        break;
      }
    }
    TileRank os[3] = { NO_TILE, NO_TILE, NO_TILE };
    extract_unmatched(info->tiles[teen_day_idx], info, os);
#define OUT_OF_BOUNDS 3
    size_t lowest_idx = OUT_OF_BOUNDS;
    for (size_t i = 0; i < OUT_OF_BOUNDS; i++) {
      if (tile_matches_class(os[i], &TC_ANY_SEVEN)) {
        lowest_idx = i;
        break;
      }
      if (tile_matches_class(os[i], &TC_ANY_EIGHT)) lowest_idx = i;
      else if (tile_matches_class(os[i], &TC_CHOPGOW)) lowest_idx = i;
    }
    if (lowest_idx < OUT_OF_BOUNDS) {
      size_t odx1 = (lowest_idx + 1) % 3;
      size_t odx2 = (lowest_idx + 2) % 3;
      if (at_least_high_3(os[odx1], os[odx2])) {
        set_hand(info, info->tiles[teen_day_idx], os[lowest_idx],
                 os[odx1], os[odx2]);
      } else if (tile_matches_class(os[odx1], &TC_ANY_EIGHT) &&
                 at_least_high_3(os[odx2], os[lowest_idx]))
            set_hand(info, info->tiles[teen_day_idx], os[odx1],
                    os[lowest_idx], os[odx2]);
        else if (tile_matches_class(os[odx2], &TC_ANY_EIGHT) &&
                 at_least_high_3(os[odx1], os[lowest_idx]))
            set_hand(info, info->tiles[teen_day_idx], os[odx2],
                     os[lowest_idx], os[odx1]);
        else if (tile_matches_class(os[odx1], &TC_CHOPGOW))
            set_hand(info, info->tiles[teen_day_idx], os[odx1],
                     os[lowest_idx], os[odx2]);
        else
            set_hand(info, info->tiles[teen_day_idx], os[odx2],
                     os[lowest_idx], os[odx1]);
    }
  }
}

typedef void (*rule_fn_t)(void);
rule_fn_t HOUSE_WAY_RULES[] = {
  rule_two_pair,
  rule_keep_pairs,
  rule_split_pair,
  rule_wong_gong_nine,
  NULL // sentinel value
};

void house_way_build(void) {
  printf("Applying house way rules... \n");
  for (size_t i = 0; HOUSE_WAY_RULES[i] != NULL; i++) HOUSE_WAY_RULES[i]();
  printf("House way rules complete.\n");
}

#endif // RULES_IMPL
#endif // RULES_H
