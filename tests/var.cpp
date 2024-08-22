#define EXTISM_CPP_IMPLEMENTATION
#include "../extism-pdk.hpp"
#include "util.h"
#include <stdint.h>

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  // test loading and setting uint32_t var
  {
    assert(!extism::var("does_not_exist"));
    uint32_t value = 0;
    if (auto var = extism::var<uint32_t>("var")) {
      auto maybe_value = var->get_stack();
      assert(maybe_value);
      value = *maybe_value;
    }
    ++value;
    assert(extism::var_set_type<uint32_t>("var", value));
    assert(extism::output(std::to_string(value)));
  }

  return 0;
}
