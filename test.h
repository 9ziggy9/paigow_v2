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
#endif
