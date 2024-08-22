#define EXTISM_CPP_IMPLEMENTATION
#include "../extism-pdk.hpp"
#include "util.h"

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  // test logging
  {
    const std::string input = extism::input().string();
    if (input == "debug") {
      extism::log_debug("debug");
    } else if (input == "info") {
      extism::log_info("info");
    } else if (input == "warn") {
      extism::log_info("warn");
    } else if (input == "error") {
      extism::log_error("error");
    } else {
      assert(0);
    }
  }

  return 0;
}
