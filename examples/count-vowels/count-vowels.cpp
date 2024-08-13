#define EXTISM_CPP_IMPLEMENTATION
#include "../../extism-pdk.hpp"

int32_t EXTISM_EXPORTED_FUNCTION(count_vowels) {
  uint64_t count = 0;
  for (auto &ch : extism::input_string()) {
    count += (ch == 'A') + (ch == 'a') + (ch == 'E') + (ch == 'e') +
             (ch == 'I') + (ch == 'i') + (ch == 'O') + (ch == 'o') +
             (ch == 'U') + (ch == 'u');
  }

  char out[128];
  int n = snprintf(out, 128, "{\"count\": %llu}", count);

  extism::output(out);

  return 0;
}
