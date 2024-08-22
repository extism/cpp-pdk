#define EXTISM_CPP_IMPLEMENTATION
#include "../extism-pdk.hpp"
#include "util.h"

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  // test loading and setting a string var
  {
    std::string value = "";
    if (auto var = extism::var<char>("var")) {
      value = var->string();
    }
    value += "a";
    assert(extism::var_set("var", value));
    assert(extism::output(value));
  }

  return 0;
}
