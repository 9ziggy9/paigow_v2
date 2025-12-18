#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "engine.h"

#define GET_NTH_TILE(hand, n) (((hand) >> ((3 - (n)) * 4)) & 0xF)
#define HAND_FROM_TILES(t1, t2, t3, t4) (((uint16_t)(t1) << 12) | \
					 ((uint16_t)(t2) << 8)  | \
					 ((uint16_t)(t3) << 4)  | \
					 ((uint16_t)(t4) << 0))
#define UNPACK_HAND_INTO_BUFFER(TS, H) do {			       \
    for (int _N = 0; _N < 4; _N++) (TS)[_N] = GET_NTH_TILE((H), (_N)); \
} while(0)

ExceptionMapping map_exception_hand(uint16_t hand, const HouseWayExceptionList xs) {
  for (size_t i = 0; i < xs.count; ++i) {
    if (hand == xs.items[i].in) {
      printf("ENGINE: exception hand detected ... \n");
      return (ExceptionMapping) {
	.is_exception = true,
	.hand = xs.items[i].out,
      };
    }
  }
  return (ExceptionMapping) {
    .is_exception = false,
    .hand = hand,
  };
}

uint16_t sort_hand_by_points(uint16_t hand) {
  uint8_t ts[4];
  UNPACK_HAND_INTO_BUFFER(ts, hand);
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

uint16_t house_way(HouseWayExceptionList xs, uint16_t in) {
  printf("ENGINE: setting hand ... \n");
  printf("ENGINE: sorting hand by points ... \n");

  uint16_t sorted = sort_hand_by_points(in);

  ExceptionMapping exception_filter = map_exception_hand(sorted, xs);
  if (exception_filter.is_exception) return exception_filter.hand;

  return sorted;
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

HouseWayExceptionList init_exception_list() {
  return (HouseWayExceptionList) {
    .items = NULL,
    .count = 0,
    .capacity = 0,
  };
}

void define_exception(HouseWayExceptionList xs, HandSpecification spec) {
}

int main(void) {
  printf("ENGINE: initializing exception list ... \n");
  HouseWayExceptionList exception_list = init_exception_list();
  return 0;
}
