#ifndef UTILS_H
#define UTILS_H

#include "engine.h"

const char *rank_to_name(TileRank rank);
void print_tiles(TileRank *ts);
void print_house_way_map(uint16_t start, uint16_t end);

// ============================================================================
// IMPLEMENTATION
// ============================================================================

#ifdef UTILS_IMPL

const char *rank_to_name(TileRank rank) {
  switch(rank) {
    case RK_TEEN:     return "TEEN";
    case RK_DAY:      return "DAY";
    case RK_YUN:      return "YUN";
    case RK_GOR:      return "GOR";
    case RK_MUUY:     return "MUUY";
    case RK_CHONG:    return "CHONG";
    case RK_BON:      return "BON";
    case RK_FU:       return "FU";
    case RK_PING:     return "PING";
    case RK_CHIT:     return "CHIT";
    case RK_LOOK:     return "LOOK";
    case RK_CHOPGOW:  return "CHOPGOW";
    case RK_CHOPBOT:  return "CHOPBOT";
    case RK_CHOPCHIT: return "CHOPCHIT";
    case RK_CHOPNG:   return "CHOPNG";
    case RK_GEEJUN:   return "GEEJUN";
    default:          return "UNKNOWN";
  }
}

void print_tiles(TileRank *ts) {
  printf(" [ ");
  for (size_t n = 0; n < 4; n++) {
    printf("%s", rank_to_name(ts[n]));
    if (n < 3) printf(" | ");
  }
  printf(" ]");
}

void print_house_way_map(uint16_t start, uint16_t end) {
  if (end > NUM_LEGAL_PERMS) end = NUM_LEGAL_PERMS;
  size_t total_bytes = sizeof(HOUSE_WAY_MAP);
  double total_kb = (double)total_bytes / 1024.0;
  
  printf("\n========================================\n");
  printf("HOUSE_WAY_MAP: %.2f KB (%zu bytes)\n", total_kb, total_bytes);
  printf("HOUSE_WAY_MAP entries [%u, %u)\n", start, end);
  printf("========================================\n");

  for (uint16_t i = start; i < end; i++) {
    struct HandInfo *info = &HOUSE_WAY_MAP[i];
    printf("[%u] 0x%04X", i, info->house_way);
    print_tiles(info->tiles);
    printf(" NUM_PAIRS: %d", info->num_pairs);
    if (info->is_splittable)
      printf(" (split on %s)", rank_to_name(info->paired_tile));
    if (info->has_gj) printf(" (GEEJUN DETECTED)");
    printf("\n");
  }
  printf("========================================\n\n");
}

#endif // UTILS_IMPL
#endif // UTILS_H
