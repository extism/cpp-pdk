#define EXTISM_CPP_IMPLEMENTATION
#include "../../extism-pdk.hpp"
#include <string>

uint64_t count = 0;

int32_t EXTISM_EXPORTED_FUNCTION(globals) {
  std::string out = "{\"count\": " + std::to_string(count) + "}";
  extism::output(out);
  count += 1;
  return 0;
}
