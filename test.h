// Just some simple testing, so far we pass
#ifdef TESTING_FUNCTION
#include <string.h>

int compare_uint16(const void *a, const void *b) {
  uint16_t ua = *(const uint16_t *)a;
  uint16_t ub = *(const uint16_t *)b;
  return (ua > ub) - (ua < ub);
}

bool sanity_check_no_duplicate_inputs(void) {
  if (HOUSE_WAY_EXCEPTIONS.count == 0) {
    printf("SANITY CHECK: No exceptions to check\n");
    return true;
  }
  
  // Create a temporary array of just the input hands
  uint16_t *inputs = malloc(HOUSE_WAY_EXCEPTIONS.count * sizeof(uint16_t));
  if (!inputs) {
    printf("SANITY CHECK: Memory allocation failed\n");
    return false;
  }
  
  for (size_t i = 0; i < HOUSE_WAY_EXCEPTIONS.count; i++) {
    inputs[i] = HOUSE_WAY_EXCEPTIONS.items[i].in;
  }
  
  // Sort the array
  qsort(inputs, HOUSE_WAY_EXCEPTIONS.count, sizeof(uint16_t), compare_uint16);
  
  // Check for adjacent duplicates
  bool has_duplicates = false;
  for (size_t i = 1; i < HOUSE_WAY_EXCEPTIONS.count; i++) {
    if (inputs[i] == inputs[i-1]) {
      printf("SANITY CHECK FAILED: Duplicate input hand found: 0x%04X\n", inputs[i]);
      print_hand(inputs[i]);
      printf("\n");
      has_duplicates = true;
    }
  }
  
  free(inputs);
  
  if (!has_duplicates) {
    printf("SANITY CHECK PASSED: All %zu input hands are unique\n", 
           HOUSE_WAY_EXCEPTIONS.count);
  }
  
  return !has_duplicates;
}

void testing_set_exceptions(void) {
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

#endif
