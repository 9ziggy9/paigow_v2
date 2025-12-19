#ifndef IO_H_
#define IO_H_

#ifdef IO_IMPL_
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

void read_raw_tiles(uint16_t ts) {
  printf("Tile: 0x%x\n", GET_NTH_TILE(ts, 0));
  printf("Tile: 0x%x\n", GET_NTH_TILE(ts, 1));
  printf("Tile: 0x%x\n", GET_NTH_TILE(ts, 2));
  printf("Tile: 0x%x\n", GET_NTH_TILE(ts, 3));
}

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

void print_hand(uint16_t hand) {
  printf("[");
  for (int i = 0; i < 4; i++) {
    TileRank tile = GET_NTH_TILE(hand, i);
    printf("%-8s", rank_to_name(tile));
    if (i < 3) printf(" | ");
  }
  printf("]");
}
#endif // IMPL

#endif // HEADER
