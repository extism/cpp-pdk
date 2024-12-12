#define EXTISM_CPP_IMPLEMENTATION
#include "../extism-pdk.hpp"
#include "util.h"
#include <stdint.h>

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  auto response = extism::http_request<char>(R"(
  {
    "method": "GET",
    "url":"https://api.fxratesapi.com/latest"
  }
  )");
  assert(response);
  assert(response->body().string().size());
  auto headers = response->headers();
  assert(headers.size());
  extism::log_debug(headers);
  return 0;
}
