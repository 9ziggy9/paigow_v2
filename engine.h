#ifndef ENGINE_H_
#define ENGINE_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct HouseWayException {
  uint16_t in;
  uint16_t out;
} HouseWayException;

typedef struct HouseWayExceptionList {
  HouseWayException *items;
  size_t count;
  size_t capacity;
} HouseWayExceptionList;

typedef enum TileRank {
  RK_TEEN     = 0xF, RK_DAY      = 0xE, RK_YUN      = 0xD,
  RK_GOR      = 0xC, RK_MUUY     = 0xB, RK_CHONG    = 0xA,
  RK_BON      = 0x9, RK_FU	     = 0x8, RK_PING     = 0x7,
  RK_CHIT     = 0x6, RK_LOOK     = 0x5, RK_CHOPGOW  = 0x4,
  RK_CHOPBOT  = 0x3, RK_CHOPCHIT = 0x2, RK_CHOPNG   = 0x1,
  RK_GEEJUN   = 0x0,
} TileRank;

typedef struct TileClass {
  uint16_t points;
  size_t   num_variants;
  const TileRank variants[2];
} TileClass;

typedef struct HouseWayRule {
  TileClass h1; TileClass h2;
  TileClass l1; TileClass l2;
} HouseWayRule;

const uint16_t RANK_TO_POINTS[16] = {
    [RK_GEEJUN]	 = 3, [RK_TEEN]     = 2, [RK_DAY]	     = 2,
    [RK_YUN]     = 8, [RK_GOR]	    = 4, [RK_MUUY]	   = 0,
    [RK_CHONG]	 = 6, [RK_BON]	    = 4, [RK_FU]	     = 1,
    [RK_PING]    = 0, [RK_CHIT]     = 7, [RK_LOOK]	   = 6,
    [RK_CHOPGOW] = 9, [RK_CHOPBOT]  = 8, [RK_CHOPCHIT] = 7,
    [RK_CHOPNG]	 = 5,
};

#define DA_APPEND(xs, x)                                                     \
  do {                                                                       \
    if ((xs).count >= (xs).capacity) {                                       \
      if ((xs).capacity == 0) (xs).capacity = 256;                           \
      else (xs).capacity *= 2;                                               \
      (xs).items = realloc((xs).items, (xs).capacity * sizeof(*(xs).items)); \
    }                                                                        \
    (xs).items[(xs).count++] = (x);                                          \
  } while(0)
#define TILE_CLASS(name, pts, n, ...) \
  const TileClass TC_##name = { pts, n, { __VA_ARGS__ } }
#define GET_NTH_TILE(hand, n) (((hand) >> ((3 - (n)) * 4)) & 0xF)
#define HAND_FROM_TILES(t1, t2, t3, t4) (((uint16_t)(t1) << 12) | \
					 ((uint16_t)(t2) << 8)  |                               \
					 ((uint16_t)(t3) << 4)  |                               \
					 ((uint16_t)(t4) << 0))
#define UNPACK_HAND_INTO_BUFFER(TS, H) do {			                      \
  for (int _N = 0; _N < 4; _N++) (TS)[_N] = GET_NTH_TILE((H), (_N));  \
} while(0)

// Single tiles.
TILE_CLASS(GEEJUN,   3, 1, RK_GEEJUN);
TILE_CLASS(TEEN,     2, 1, RK_TEEN);
TILE_CLASS(DAY,      2, 1, RK_DAY);
TILE_CLASS(YUN,      8, 1, RK_YUN);
TILE_CLASS(GOR,      4, 1, RK_GOR);
TILE_CLASS(MUUY,     0, 1, RK_MUUY);
TILE_CLASS(CHONG,    6, 1, RK_CHONG);
TILE_CLASS(BON,      8, 1, RK_BON);
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


typedef struct ExceptionMapping {
  bool is_exception;
  uint16_t hand;
} ExceptionMapping;

static uint16_t sort_hand_by_points(uint16_t hand) {
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

/* GLOBAL BUT INACCESSIBLE IN MAIN */
static HouseWayExceptionList HOUSE_WAY_EXCEPTIONS = {
    .items    = NULL,
    .count    = 0,
    .capacity = 0,
};

#ifdef ENGINE_IMPL
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

void initialize_exceptions(void) {
  printf("ENGINE: initializing exception list ... \n");
  // 9/11 rule for teen/day
  define_exception((HouseWayRule){TC_TEEN, TC_CHOPGOW, TC_TEEN, TC_FU});
  define_exception((HouseWayRule){TC_DAY, TC_CHOPGOW, TC_DAY, TC_FU});

  // Gee Jun splits 4-6, 5-6, 6-6
  define_exception((HouseWayRule){
      TC_GEEJUN, TC_ANY_SIX, TC_GEEJUN, TC_ANY_FOUR
    });
  define_exception((HouseWayRule){TC_GEEJUN, TC_ANY_SIX, TC_GEEJUN, TC_CHOPNG});
  define_exception((HouseWayRule){TC_GEEJUN, TC_CHONG  , TC_GEEJUN, TC_LOOK});

  // Split 9s with any combination of 2, 10, 12 or with non pairing 10s
  define_exception((HouseWayRule){
      TC_TEEN_OR_DAY, TC_CHOPGOW, TC_ANY_TEN, TC_CHOPGOW
    });
  define_exception((HouseWayRule){TC_TEEN, TC_CHOPGOW, TC_DAY, TC_CHOPGOW});
  define_exception((HouseWayRule){TC_MUUY, TC_CHOPGOW, TC_PING, TC_CHOPGOW});

  // Split 8s with any combination of 2, 10, 11, 12, non pairing 10s or 9/11.
  // Need to handle both cases individually.
  define_exception((HouseWayRule){
      TC_TEEN_OR_DAY, TC_CHOPBOT, TC_ANY_TEN, TC_CHOPBOT
    });
  define_exception((HouseWayRule){TC_TEEN, TC_CHOPBOT, TC_DAY, TC_CHOPBOT});
  define_exception((HouseWayRule){
      TC_TEEN_OR_DAY, TC_CHOPBOT, TC_FU, TC_CHOPBOT
    });
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
  define_exception((HouseWayRule){
      TC_TEEN_OR_DAY, TC_CHIT, TC_ANY_TEN, TC_CHIT
    });
  define_exception((HouseWayRule){TC_TEEN_OR_DAY, TC_CHIT, TC_FU, TC_CHIT});
  define_exception((HouseWayRule){TC_TEEN, TC_CHOPCHIT, TC_DAY, TC_CHOPCHIT});
  define_exception((HouseWayRule){
      TC_TEEN_OR_DAY, TC_CHOPCHIT, TC_ANY_TEN, TC_CHOPCHIT
    });
  define_exception((HouseWayRule){
      TC_TEEN_OR_DAY, TC_CHOPCHIT, TC_FU, TC_CHOPCHIT
    });
  printf("ENGINE: exception list initialized successfully ... \n");
}

#endif // IMPL
#endif // HEADER
