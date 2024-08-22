#define EXTISM_CPP_IMPLEMENTATION
#include "../extism-pdk.hpp"
#include "util.h"
#include <stdint.h>

struct Input {
  char data[14];
};

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  // test loading full config
  {
    assert(!extism::config("does_not_exist"));
    assert(extism::config("key"));
    assert(extism::config("key")->string() == "this is a test");
    auto vec = extism::config("key")->vec();
    assert(std::string(vec.begin(), vec.end()) == "this is a test");
    auto maybe_config = extism::config<struct Input>("key")->get_stack();
    assert(maybe_config);
    assert(memcmp("this is a test", maybe_config->data, 14) == 0);
    auto config_ptr = extism::config<struct Input>("key")->get();
    assert(config_ptr);
    assert(memcmp("this is a test", config_ptr->data, 14) == 0);
    auto uintvec = extism::config<uint32_t>("key")->vec();
    assert(uintvec.size() == 3);
  }

  return 0;
}
