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

  // 9/11 rule for teen/day
  define_exception((HouseWayRule){TC_TEEN, TC_CHOPGOW, TC_TEEN, TC_FU});
  define_exception((HouseWayRule){TC_DAY, TC_CHOPGOW, TC_DAY, TC_FU});

  // Gee Jun splits 4-6, 5-6, 6-6
  define_exception((HouseWayRule){TC_GEEJUN, TC_ANY_SIX, TC_GEEJUN, TC_ANY_FOUR});
  define_exception((HouseWayRule){TC_GEEJUN, TC_ANY_SIX, TC_GEEJUN, TC_CHOPNG});
  define_exception((HouseWayRule){TC_GEEJUN, TC_CHONG  , TC_GEEJUN, TC_LOOK});

  // Split 9s with any combination of 2, 10, 12 or with non pairing 10s
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHOPGOW, TC_ANY_TEN, TC_CHOPGOW});
  define_exception((HouseWayRule){TC_TEEN, TC_CHOPGOW, TC_DAY, TC_CHOPGOW});
  define_exception((HouseWayRule){TC_MUUY, TC_CHOPGOW, TC_PING, TC_CHOPGOW});

  // Split 8s with any combination of 2, 10, 11, 12, non pairing 10s or 9/11.
  // Need to handle both cases individually.
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHOPBOT, TC_ANY_TEN, TC_CHOPBOT});
  define_exception((HouseWayRule){TC_TEEN, TC_CHOPBOT, TC_DAY, TC_CHOPBOT});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHOPBOT, TC_FU, TC_CHOPBOT});
  define_exception((HouseWayRule){TC_FU, TC_CHOPBOT, TC_ANY_TEN, TC_CHOPBOT});
  define_exception((HouseWayRule){TC_FU, TC_CHOPBOT, TC_CHOPGOW, TC_CHOPBOT});
  define_exception((HouseWayRule){TC_MUUY, TC_CHOPBOT, TC_PING, TC_CHOPBOT});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_YUN, TC_YUN, TC_ANY_TEN});
  define_exception((HouseWayRule){TC_TEEN, TC_YUN, TC_DAY, TC_YUN});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_YUN, TC_YUN, TC_FU});
  define_exception((HouseWayRule){TC_YUN, TC_FU, TC_YUN, TC_ANY_TEN});
  define_exception((HouseWayRule){TC_YUN, TC_FU, TC_YUN, TC_CHOPGOW});
  define_exception((HouseWayRule){TC_YUN, TC_MUUY, TC_YUN, TC_PING});

  // Split 7s with 2/12 and 10/11, handle both cases individually.
  define_exception((HouseWayRule){TC_TEEN, TC_CHIT, TC_DAY, TC_CHIT});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHIT, TC_ANY_TEN, TC_CHIT});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHIT, TC_FU, TC_CHIT});
  define_exception((HouseWayRule){TC_TEEN, TC_CHOPCHIT, TC_DAY, TC_CHOPCHIT});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHOPCHIT, TC_ANY_TEN, TC_CHOPCHIT});
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHOPCHIT, TC_FU, TC_CHOPCHIT});

  

  printf("ENGINE: exception list initialized successfully ... \n");
}


typedef struct AnalysisPairs {
  bool has_pairs;
  size_t num_pairs;
  TileRank tiles[4];
  size_t pair_pos[2];
  uint16_t hand;
} AnalysisPairs;

// Very interestingly, XOR(2) has property:
// 0 -> 2, 1 -> 3, 2 -> 0, 3 -> 1
// see: https://en.wikipedia.org/wiki/Exclusive_or
AnalysisPairs map_paired_hand(uint16_t hand) {
  AnalysisPairs info = {
    .has_pairs = false,
    .num_pairs = 0,
    .tiles     = {0},
    .pair_pos  = {0},
    .hand      = 0,
  };

  UNPACK_HAND_INTO_BUFFER(info.tiles, hand);

  for (size_t n = 0; n < 3; n++) {
    if (info.tiles[n] == info.tiles[n + 1]) {
      info.has_pairs = true;
      info.pair_pos[info.num_pairs++] = n;
    }
  }

  // Refactor me.
  if (info.has_pairs) {
    size_t p1 = info.pair_pos[0];
    size_t p2 = info.pair_pos[0] + 1;
    if (info.num_pairs > 1 && info.tiles[p1] < info.tiles[p1^2]) {
        info.hand = (uint16_t)HAND_FROM_TILES(info.tiles[p1 ^ 2],
                                              info.tiles[p2 ^ 2],
                                              info.tiles[p1],
                                              info.tiles[p2]);
    } else {
        info.hand = (uint16_t)HAND_FROM_TILES(info.tiles[p1],
                                              info.tiles[p2],
                                              info.tiles[p1 ^ 2],
                                              info.tiles[p2 ^ 2]);
    }
  }

// DEBUG
#if 1
  printf("ENGINE: unpacked --> 0x%x 0x%x 0x%x 0x%x\n",
         info.tiles[0],
         info.tiles[1],
         info.tiles[2],
         info.tiles[3]
         );
  if (info.has_pairs) {
    printf("ENGINE: hand has %zu pairs.\n", info.num_pairs);
    for (size_t i = 0; i < info.num_pairs; i++) {
      printf("ENGINE: pair %-8s @ %zu\n",
             rank_to_name(info.tiles[info.pair_pos[i]]),
	     info.pair_pos[i]
             );
    }
  }
#endif

  return info;
}

uint16_t house_way(uint16_t in) {
  printf("ENGINE: setting hand ... \n");
  printf("ENGINE: sorting hand by points ... \n");

  uint16_t sorted = sort_hand_by_points(in);
  printf("ENGINE: sorted hand: 0x%4x ... \n", sorted);

  ExceptionMapping exception_filter = map_exception_hand(sorted);
  if (exception_filter.is_exception) return exception_filter.hand;

  AnalysisPairs pair_filter = map_paired_hand(sorted);
  if (pair_filter.has_pairs) return pair_filter.hand;

  return sorted;
}

int main(void) {
  printf("ENGINE: entering main ... \n");
  initialize_exceptions();
  return 0;
}
