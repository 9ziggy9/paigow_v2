#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum TileRank {
  RK_TEEN    = 0xF, RK_DAY      = 0xE, RK_YUN     = 0xD,
  RK_GOR     = 0xC, RK_MUUY     = 0xB, RK_CHONG   = 0xA,
  RK_BON     = 0x9, RK_FU	      = 0x8, RK_PING    = 0x7,
  RK_CHIT    = 0x6, RK_LOOK     = 0x5, RK_CHOPGOW = 0x4,
  RK_CHOPBOT = 0x3, RK_CHOPCHIT = 0x2, RK_CHOPNG  = 0x1,
  RK_GEEJUN  = 0x0,
} TileRank;

#define NO_TILE 0xFF

#define HAND_FROM_TILES(t1, t2, t3, t4) (((uint16_t)(t1) << 12) | \
					 ((uint16_t)(t2) << 8)  |                               \
					 ((uint16_t)(t3) << 4)  |                               \
					 ((uint16_t)(t4) << 0))

#define GET_NTH_TILE(hand, n) (((hand) >> ((3 - (n)) * 4)) & 0xF)

// CORE ENGINE DATA
struct HandInfo {
  uint16_t house_way;
  TileRank tiles[4];
  bool has_gj;
  bool is_exception;
  uint8_t num_pairs;
  bool is_splittable;
  TileRank paired_tile;
};

// Constants
#define NUM_PERMS 65536
#define NUM_LEGAL_PERMS 3620
#define INVALID_HAND 0xFFFF

// Global data declarations
extern uint16_t VALID_HAND_IDXS[NUM_PERMS];
extern struct HandInfo HOUSE_WAY_MAP[NUM_LEGAL_PERMS];

// Hash function type
typedef uint16_t (*hash_fn_t)(uint16_t);
extern hash_fn_t hash;

// Function declarations
void extract_tiles(uint16_t hand, struct HandInfo *info);
uint16_t sort_hand(uint16_t hand);
void house_way_init(void);
struct HandInfo *get_hand_info(uint16_t hand);

// ============================================================================
// IMPLEMENTATION
// ============================================================================

#ifdef ENGINE_IMPL

// Global data definitions
uint16_t VALID_HAND_IDXS[NUM_PERMS];
struct HandInfo HOUSE_WAY_MAP[NUM_LEGAL_PERMS];

// Default hash using auxiliary array
static uint16_t hash_via_lookup(uint16_t hand) {
  return VALID_HAND_IDXS[hand];
}

// Global hash function pointer
hash_fn_t hash = hash_via_lookup;

void extract_tiles(uint16_t hand, struct HandInfo *info) {
  for (size_t n = 0; n < 4; n++) info->tiles[n] = GET_NTH_TILE(hand, n);
}

uint16_t sort_hand(uint16_t hand) {
  uint8_t tiles[4];
  
  tiles[0] = GET_NTH_TILE(hand, 0);
  tiles[1] = GET_NTH_TILE(hand, 1);
  tiles[2] = GET_NTH_TILE(hand, 2);
  tiles[3] = GET_NTH_TILE(hand, 3);
  
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3 - i; j++) {
      if (tiles[j] < tiles[j + 1]) {
        uint8_t tmp = tiles[j];
        tiles[j] = tiles[j + 1];
        tiles[j + 1] = tmp;
      }
    }
  }
  
  return HAND_FROM_TILES(tiles[0], tiles[1], tiles[2], tiles[3]);
}

void house_way_init(void) {
  for (size_t i = 0; i < NUM_PERMS; i++) {
    VALID_HAND_IDXS[i] = INVALID_HAND;
  }
  
  TileRank deck[32] = {
    RK_GEEJUN, RK_GEEJUN,
    RK_CHOPNG, RK_CHOPNG,
    RK_CHOPCHIT, RK_CHOPCHIT,
    RK_CHOPBOT, RK_CHOPBOT,
    RK_CHOPGOW, RK_CHOPGOW,
    RK_LOOK, RK_LOOK,
    RK_CHIT, RK_CHIT,
    RK_PING, RK_PING,
    RK_FU, RK_FU,
    RK_BON, RK_BON,
    RK_CHONG, RK_CHONG,
    RK_MUUY, RK_MUUY,
    RK_GOR, RK_GOR,
    RK_YUN, RK_YUN,
    RK_DAY, RK_DAY,
    RK_TEEN, RK_TEEN,
  };
  
  uint16_t hand_count = 0;
  
  // Generate all C(32,4) = 35,960 combinations, but only keep unique ones
  for (size_t i = 0; i < 32; i++) {
    for (size_t j = i + 1; j < 32; j++) {
      for (size_t k = j + 1; k < 32; k++) {
        for (size_t l = k + 1; l < 32; l++) {
          uint16_t hand = HAND_FROM_TILES(deck[i], deck[j], deck[k], deck[l]);
          uint16_t sorted = sort_hand(hand);
          if (VALID_HAND_IDXS[sorted] == INVALID_HAND) {
            VALID_HAND_IDXS[sorted] = hand_count;
            extract_tiles(sorted, &HOUSE_WAY_MAP[hand_count]);
            HOUSE_WAY_MAP[hand_count].house_way     = sorted;
            HOUSE_WAY_MAP[hand_count].has_gj        = false;
            HOUSE_WAY_MAP[hand_count].is_exception  = false;
            HOUSE_WAY_MAP[hand_count].num_pairs     = 0;
            HOUSE_WAY_MAP[hand_count].is_splittable = false;
            HOUSE_WAY_MAP[hand_count].paired_tile   = NO_TILE;
            hand_count++;
          }
        }
      }
    }
  }
  
  printf("Generated %u unique hands\n", hand_count);
}

struct HandInfo *get_hand_info(uint16_t hand) {
  uint16_t sorted = sort_hand(hand);
  uint16_t idx = hash(sorted);
  if (idx == INVALID_HAND) return NULL;
  return &HOUSE_WAY_MAP[idx];
}

#endif // ENGINE_IMPL
#endif // ENGINE_H
