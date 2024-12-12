#define EXTISM_CPP_IMPLEMENTATION
#include "../extism-pdk.hpp"
#include "util.h"

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  // test logging
  {
    extism::log_trace("trace");
    extism::log_debug("debug");
    extism::log_info("info");
    extism::log_warn("warn");
    extism::log_error("error");
  }

  return 0;
}
