#define EXTISM_CPP_IMPLEMENTATION
#include "../../extism-pdk.hpp"

inline unsigned is_vowel(char ch) {
  return (ch == 'A') + (ch == 'a') + (ch == 'E') + (ch == 'e') + (ch == 'I') +
         (ch == 'i') + (ch == 'O') + (ch == 'o') + (ch == 'U') + (ch == 'u');
}

int32_t EXTISM_EXPORTED_FUNCTION(count_vowels) {
  uint64_t count = 0;
  for (auto &ch : extism::input().string()) {
    count += is_vowel(ch);
  }

  /*for (auto &ch : extism::input_vec<char>()) {
    count += is_vowel(ch);
  }

  typedef struct __attribute__((packed)) {
    char str[14];
  } InputType;

  // struct stack
  auto input = extism::input_stack<InputType>();
  for (int i = 0; i < sizeof(InputType); i++) {
    count += is_vowel(input.str[i]);
  }

  // struct heap
  auto pinput = extism::input<InputType>();
  for (int i = 0; i < sizeof(InputType); i++) {
    count += is_vowel(pinput->str[i]);
  }
  */

  char out[128];
  int n = snprintf(out, 128, "{\"count\": %llu}", count);
  extism::output(out);

  return 0;
}
