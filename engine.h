#include <stdlib.h>
#include <stdint.h>

typedef enum TileRank {
  RK_TEEN     = 0xF, RK_DAY      = 0xE, RK_YUN      = 0xD,
  RK_GOR      = 0xC, RK_MUUY     = 0xB, RK_CHONG    = 0xA,
  RK_BON      = 0x9, RK_FU	 = 0x8, RK_PING     = 0x7,
  RK_CHIT     = 0x6, RK_LOOK     = 0x5, RK_CHOPGOW  = 0x4,
  RK_CHOPBOT  = 0x3, RK_CHOPCHIT = 0x2, RK_CHOPNG   = 0x1,
  RK_GEEJUN   = 0x0,
} TileRank;

const uint16_t RANK_TO_POINTS[16] = {
    [RK_GEEJUN]	  = 3, [RK_TEEN]     = 2, [RK_DAY]	= 2,
    [RK_YUN]	  = 8, [RK_GOR]	     = 4, [RK_MUUY]	= 0,
    [RK_CHONG]	  = 6, [RK_BON]	     = 4, [RK_FU]	= 1,
    [RK_PING]	  = 0, [RK_CHIT]     = 7, [RK_LOOK]	= 6,
    [RK_CHOPGOW]  = 9, [RK_CHOPBOT]  = 8, [RK_CHOPCHIT] = 7,
    [RK_CHOPNG]	  = 5,
};

typedef struct TileClass {
  uint16_t points;
  size_t   num_variants;
  const TileRank variants[2];
} TileClass;

#define TILE_CLASS(name, pts, n, ...)			  \
  const TileClass TC_##name = { pts, n, { __VA_ARGS__ } } \

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
TILE_CLASS(ANY_TEN,   0, 2, RK_MUUY, RK_PING);
TILE_CLASS(ANY_EIGHT, 8, 2, RK_YUN, RK_CHOPBOT);
TILE_CLASS(ANY_SEVEN, 7, 2, RK_CHIT, RK_CHOPCHIT);
TILE_CLASS(ANY_SIX,   6, 2, RK_CHONG, RK_LOOK);
TILE_CLASS(ANY_FOUR,  4, 2, RK_GOR, RK_BON);

typedef struct HouseWayException {
  uint16_t in;
  uint16_t out;
} HouseWayException;

typedef struct HouseWayExceptionList {
  HouseWayException *items;
  size_t count;
  size_t capacity;
} HouseWayExceptionList;

typedef struct HandSpecification {
  TileClass h1; TileClass h2;
  TileClass l1; TileClass l2;
} HandSpecification;

typedef struct HouseWayRule {
  HandSpecification in;
  HandSpecification out;
} HouseWayRule;

#define DA_APPEND(xs, x) {			      	                        \
  do {						      		                \
    if ((xs)->count >= (xs)->capacity) {				        \
      if ((xs)->capacity == 0) (xs)->capacity = 256;		                \
      else (xs)->capacity *= 2;				                        \
      (xs)->items = realloc((xs)->items, (xs)->capacity * sizeof(*(xs)->items); \
    }								                \
    (xs)->items[(xs)->count++] = (x);				                \
  } while(0)


typedef struct ExceptionMapping {
  bool is_exception;
  uint16_t hand;
} ExceptionMapping;
