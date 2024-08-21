#define EXTISM_CPP_IMPLEMENTATION
#include "../../extism-pdk.hpp"

#include <stdio.h>

EXTISM_IMPORT("extism:host/user", "hello_world")
extern extism::imports::RawHandle hello_world(extism::imports::RawHandle);

inline unsigned is_vowel(char ch) {
  return (ch == 'A') + (ch == 'a') + (ch == 'E') + (ch == 'e') + (ch == 'I') +
         (ch == 'i') + (ch == 'O') + (ch == 'o') + (ch == 'U') + (ch == 'u');
}

int32_t EXTISM_EXPORTED_FUNCTION(count_vowels) {
  uint64_t count = 0;
  for (auto &ch : extism::input_string()) {
    count += is_vowel(ch);
  }

  char out[128];
  int n = snprintf(out, 128, "{\"count\": %lld}", count);
  auto hello_input = extism::UniqueHandle<char>::from(out);
  if (!hello_input) {
    return 1;
  }
  auto hello_result = hello_world(*hello_input);
  if (!hello_result) {
    return 2;
  }
  extism::output<char>(extism::UniqueHandle<char>(hello_result));
  return 0;
}
