#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "engine.h"

// GLOBALS
HouseWayExceptionList HOUSE_WAY_EXCEPTIONS = {
  .items    = NULL,
  .count    = 0,
  .capacity = 0,
};

#define GET_NTH_TILE(hand, n) (((hand) >> ((3 - (n)) * 4)) & 0xF)
#define HAND_FROM_TILES(t1, t2, t3, t4) (((uint16_t)(t1) << 12) | \
					 ((uint16_t)(t2) << 8)  | \
					 ((uint16_t)(t3) << 4)  | \
					 ((uint16_t)(t4) << 0))
#define UNPACK_HAND_INTO_BUFFER(TS, H) do {			       \
    for (int _N = 0; _N < 4; _N++) (TS)[_N] = GET_NTH_TILE((H), (_N)); \
} while(0)

uint16_t sort_hand_by_points(uint16_t hand) {
  uint8_t ts[4];
  UNPACK_HAND_INTO_BUFFER(ts, hand);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3 - i; j++) {
      if (RANK_TO_POINTS[ts[j]] < RANK_TO_POINTS[ts[j + 1]]) {
	uint8_t tmp = ts[j];
	ts[j] = ts[j+1];
	ts[j+1] = tmp;
	// if equal, we need to additionally sort by pair ranking
      } else if (RANK_TO_POINTS[ts[j]] == RANK_TO_POINTS[ts[j + 1]]) {
        if (ts[j] < ts[j + 1]) {
	  uint8_t tmp = ts[j];
	  ts[j] = ts[j+1];
	  ts[j+1] = tmp;
        }
      }
    }
  }
  return (uint16_t) HAND_FROM_TILES(ts[0], ts[1], ts[2], ts[3]);
}

void read_raw_tiles(uint16_t ts) {
  printf("Tile: 0x%x\n", GET_NTH_TILE(ts, 0));
  printf("Tile: 0x%x\n", GET_NTH_TILE(ts, 1));
  printf("Tile: 0x%x\n", GET_NTH_TILE(ts, 2));
  printf("Tile: 0x%x\n", GET_NTH_TILE(ts, 3));
}

void wasm_ok(void) {
  printf("ENGINE: successfully initialized cFFI and engine.\n");
}

void print_hand(uint16_t hand) {
  printf("[");
  for (int i = 0; i < 4; i++) {
    TileRank tile = GET_NTH_TILE(hand, i);
    printf("%-8s", rank_to_name(tile));
    if (i < 3) printf(" | ");
  }
  printf("]");
}

void pretty_print_exceptions(const HouseWayExceptionList *xs) {
  printf("\n========================================\n");
  printf("House Way Exception List\n");
  printf("========================================\n");
  printf("Total Exceptions: %zu\n", xs->count);
  printf("Capacity: %zu\n", xs->capacity);
  printf("========================================\n\n");
  
  for (size_t i = 0; i < xs->count; i++) {
    printf("Exception #%zu (raw: 0x%4x -> 0x%4x):\n",
           i + 1, xs->items[i].in, xs->items[i].out);
    printf("  IN:  ");
    print_hand(xs->items[i].in);
    printf("\n");
    printf("  OUT: ");
    print_hand(xs->items[i].out);
    printf("\n\n");
  }
  
  printf("========================================\n");
}

ExceptionMapping map_exception_hand(uint16_t hand) {
  printf("ENGINE: checking exception list ... \n");
  for (size_t i = 0; i < HOUSE_WAY_EXCEPTIONS.count; ++i) {
    if (hand == HOUSE_WAY_EXCEPTIONS.items[i].in) {
      printf("ENGINE: exception hand detected ... \n");
      return (ExceptionMapping) {
	.is_exception = true,
	.hand = HOUSE_WAY_EXCEPTIONS.items[i].out,
      };
    }
  }
  printf("ENGINE: hand is not an exception ... \n");
  return (ExceptionMapping) {
    .is_exception = false,
    .hand = hand,
  };
}

uint16_t house_way(uint16_t in) {
  printf("ENGINE: setting hand ... \n");
  printf("ENGINE: sorting hand by points ... \n");

  uint16_t sorted = sort_hand_by_points(in);
  printf("ENGINE: sorted hand: 0x%4x ... \n", sorted);

  ExceptionMapping exception_filter = map_exception_hand(sorted);
  if (exception_filter.is_exception) return exception_filter.hand;

  return sorted;
}

void define_exception(HouseWayRule rule) {
  const TileClass *h1 = &rule.h1;
  const TileClass *h2 = &rule.h2;
  const TileClass *l1 = &rule.l1;
  const TileClass *l2 = &rule.l2;

  // This looks like a lot of time complexity, but we are looking at
  // 2 possible elements at each level, no more.
  for (size_t i_h1 = 0; i_h1 < h1->num_variants; i_h1++) {
    for (size_t i_h2 = 0; i_h2 < h2->num_variants; i_h2++) {
      for (size_t i_l1 = 0; i_l1 < l1->num_variants; i_l1++) {
	for (size_t i_l2 = 0; i_l2 < l2->num_variants; i_l2++) {
          TileRank ts[4] = {
	    h1->variants[i_h1],
	    h2->variants[i_h2],
	    l1->variants[i_l1],
	    l2->variants[i_l2]
          };

	  uint16_t out = (uint16_t) HAND_FROM_TILES(ts[0], ts[1], ts[2], ts[3]);
	  uint16_t in  = sort_hand_by_points(out);
          HouseWayException x = { in, out };
	  DA_APPEND(HOUSE_WAY_EXCEPTIONS, x);
	}
      }
    }
  }
}

void initialize_exceptions(void) {
  printf("ENGINE: initializing exception list ... \n");
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_ANY_EIGHT, TC_ANY_SEVEN, TC_ANY_FOUR});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHOPGOW, TC_ANY_SEVEN, TC_BON});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_ANY_EIGHT, TC_CHOPGOW, TC_ANY_FOUR});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHOPGOW, TC_FU, TC_ANY_SEVEN});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHOPGOW, TC_FU, TC_ANY_EIGHT});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_ANY_FOUR, TC_FU, TC_CHOPNG});
  define_exception((HouseWayRule){TC_ANY_SIX, TC_GEEJUN, TC_TEEN_OR_DAY, TC_CHOPNG});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_LOOK, TC_CHONG, TC_ANY_FOUR});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_LOOK, TC_CHONG, TC_CHOPNG});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHOPNG, TC_MUUY, TC_PING});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_ANY_SIX, TC_MUUY, TC_PING});
  define_exception((HouseWayRule){TC_DAY, TC_CHOPNG, TC_TEEN, TC_ANY_FOUR});
  define_exception((HouseWayRule){TC_DAY, TC_ANY_SIX, TC_TEEN, TC_ANY_FOUR});
  define_exception((HouseWayRule){TC_TEEN, TC_ANY_SIX, TC_DAY, TC_CHOPNG});
  define_exception((HouseWayRule){TC_DAY, TC_FU, TC_TEEN, TC_ANY_TEN});
  define_exception((HouseWayRule){TC_FU, TC_ANY_SIX, TC_MUUY, TC_PING});
  define_exception((HouseWayRule){TC_FU, TC_ANY_SEVEN, TC_MUUY, TC_PING});
  define_exception((HouseWayRule){TC_FU, TC_ANY_EIGHT, TC_MUUY, TC_PING});
  define_exception((HouseWayRule){TC_MUUY, TC_CHOPGOW, TC_FU, TC_PING});
  define_exception((HouseWayRule){TC_ANY_FOUR, TC_PING, TC_MUUY, TC_FU});
  define_exception((HouseWayRule){TC_MUUY, TC_CHOPGOW, TC_CHONG, TC_PING});
  define_exception((HouseWayRule){TC_MUUY, TC_ANY_SEVEN, TC_CHONG, TC_PING});
  define_exception((HouseWayRule){TC_MUUY, TC_CHOPGOW, TC_PING, TC_ANY_EIGHT});
  define_exception((HouseWayRule){TC_YUN, TC_FU, TC_MUUY, TC_ANY_SIX});
  define_exception((HouseWayRule){TC_YUN, TC_PING, TC_FU, TC_ANY_SIX});
  define_exception((HouseWayRule){TC_YUN, TC_FU, TC_ANY_TEN, TC_ANY_SEVEN});
  define_exception((HouseWayRule){TC_YUN, TC_FU, TC_ANY_TEN, TC_CHOPBOT});
  define_exception((HouseWayRule){TC_YUN, TC_ANY_SIX, TC_GOR, TC_CHOPBOT});
  define_exception((HouseWayRule){TC_CHOPBOT, TC_ANY_SIX, TC_YUN, TC_BON});
  define_exception((HouseWayRule){TC_YUN, TC_ANY_SEVEN, TC_CHOPBOT, TC_GOR});
  define_exception((HouseWayRule){TC_YUN, TC_ANY_SEVEN, TC_CHONG, TC_CHOPBOT});
  define_exception((HouseWayRule){TC_CHONG, TC_GEEJUN, TC_LOOK, TC_ANY_FOUR});
  define_exception((HouseWayRule){TC_CHONG, TC_GEEJUN, TC_LOOK, TC_CHOPNG});
  define_exception((HouseWayRule){TC_CHONG, TC_GEEJUN, TC_FU, TC_LOOK});
  define_exception((HouseWayRule){TC_CHONG, TC_FU, TC_LOOK, TC_ANY_FOUR});
  define_exception((HouseWayRule){TC_CHONG, TC_FU, TC_LOOK, TC_CHOPNG});
  define_exception((HouseWayRule){TC_GOR, TC_CHOPNG, TC_BON, TC_GEEJUN});
  define_exception((HouseWayRule){TC_GOR, TC_CHOPNG, TC_BON, TC_ANY_SIX});
  define_exception((HouseWayRule){TC_GOR, TC_CHOPNG, TC_BON, TC_ANY_SEVEN});
  define_exception((HouseWayRule){TC_GOR, TC_CHOPNG, TC_BON, TC_CHOPBOT});
  define_exception((HouseWayRule){TC_GOR, TC_BON, TC_YUN, TC_CHOPNG});
  define_exception((HouseWayRule){TC_BON, TC_CHOPNG, TC_GOR, TC_CHOPGOW});
  define_exception((HouseWayRule){TC_GOR, TC_BON, TC_ANY_TEN, TC_CHOPNG});
  define_exception((HouseWayRule){TC_GOR, TC_BON, TC_TEEN_OR_DAY, TC_CHOPNG});
  define_exception((HouseWayRule){TC_BON, TC_FU, TC_GOR, TC_PING});
  define_exception((HouseWayRule){TC_GOR, TC_FU, TC_BON, TC_MUUY});
  define_exception((HouseWayRule){TC_LOOK, TC_GEEJUN, TC_ANY_FOUR, TC_ANY_SEVEN});
  define_exception((HouseWayRule){TC_LOOK, TC_GEEJUN, TC_ANY_SEVEN, TC_CHOPNG});
  define_exception((HouseWayRule){TC_ANY_FOUR, TC_GEEJUN, TC_YUN, TC_CHOPBOT});
  define_exception((HouseWayRule){TC_ANY_FOUR, TC_FU, TC_ANY_SIX, TC_ANY_SEVEN});
  define_exception((HouseWayRule){TC_ANY_TEN, TC_ANY_FOUR, TC_ANY_SIX, TC_ANY_SEVEN});
  define_exception((HouseWayRule){TC_MUUY, TC_CHOPNG, TC_ANY_FOUR, TC_PING});
  define_exception((HouseWayRule){TC_PING, TC_ANY_EIGHT, TC_MUUY, TC_ANY_SEVEN});
  define_exception((HouseWayRule){TC_ANY_FOUR, TC_FU, TC_ANY_TEN, TC_CHOPNG});
  printf("ENGINE: exception list initialized successfully ... \n");
}

int main(void) {
  initialize_exceptions();
  return 0;
}
