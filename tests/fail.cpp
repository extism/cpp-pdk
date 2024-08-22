#define EXTISM_CPP_IMPLEMENTATION
#include "../extism-pdk.hpp"
#include "util.h"

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  extism::error_set("Some error message");
  return 1;
}
