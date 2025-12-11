#include <stdint.h>
#include <stdio.h>

typedef enum TileRank {
  RK_TEEN     = 0xF,
  RK_DAY      = 0xE,
  RK_YUN      = 0xD,
  RK_GOR      = 0xC,
  RK_MUUY     = 0xB,
  RK_CHONG    = 0xA,
  RK_BON      = 0x9,
  RK_FU	      = 0x8,
  RK_PING     = 0x7,
  RK_CHIT     = 0x6,
  RK_LOOK     = 0x5,
  RK_CHOPGOW  = 0x4,
  RK_CHOPBOT  = 0x3,
  RK_CHOPCHIT = 0x2,
  RK_CHOPNG   = 0x1,
  RK_GEEJUN   = 0x0,
} TileRank;

const uint16_t RANK_TO_POINTS[16] = {
    [RK_GEEJUN]	  = 3,
    [RK_TEEN]	  = 2,
    [RK_DAY]	  = 2,
    [RK_YUN]	  = 8,
    [RK_GOR]	  = 4,
    [RK_MUUY]	  = 0,
    [RK_CHONG]	  = 6,
    [RK_BON]	  = 4,
    [RK_FU]	  = 1,
    [RK_PING]	  = 0,
    [RK_CHIT]	  = 7,
    [RK_LOOK]	  = 6,
    [RK_CHOPGOW]  = 9,
    [RK_CHOPBOT]  = 8,
    [RK_CHOPCHIT] = 7,
    [RK_CHOPNG]	  = 5,
};

#define GET_NTH_TILE(hand, n) (((hand) >> ((3 - (n)) * 4)) & 0xF)
#define HAND_FROM_TILES(t1, t2, t3, t4) (((uint16_t)(t1) << 12) | \
					 ((uint16_t)(t2) << 8)  | \
					 ((uint16_t)(t3) << 4)  | \
					 ((uint16_t)(t4) << 0))
#define UNPACK_HAND_INTO(TS, H) do {				       \
    for (int _N = 0; _N < 4; _N++) (TS)[_N] = GET_NTH_TILE((H), (_N)); \
} while(0)

uint16_t sort_hand_by_points(uint16_t hand) {
  uint8_t ts[4];
  UNPACK_HAND_INTO(ts, hand);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3 - i; j++) {
      if (RANK_TO_POINTS[ts[j]] < RANK_TO_POINTS[ts[j + 1]]) {
	uint8_t tmp = ts[j];
	ts[j] = ts[j+1];
	ts[j+1] = tmp;
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
  printf("Successfully initialized cFFI and engine.\n");
}

int main(void) {
  printf("main.c is loaded\n");
  return 0;
}
