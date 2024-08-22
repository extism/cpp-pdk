#define EXTISM_CPP_IMPLEMENTATION
#include "../extism-pdk.hpp"
#include "util.h"
#include <stdint.h>

struct Input {
  char data[14];
};

int32_t EXTISM_EXPORTED_FUNCTION(run_test) {
  // test loading full input
  {
    assert(extism::input().string() == "this is a test");
    auto vec = extism::input().vec();
    assert(std::string(vec.begin(), vec.end()) == "this is a test");
    auto maybe_input = extism::input<struct Input>().get_stack();
    assert(maybe_input);
    assert(memcmp("this is a test", maybe_input->data, 14) == 0);
    auto input_ptr = extism::input<struct Input>().get();
    assert(input_ptr);
    assert(memcmp("this is a test", input_ptr->data, 14) == 0);
    auto uintvec = extism::input<uint32_t>().vec();
    assert(uintvec.size() == 3);
  }

  // test loading partial input
  {
    auto maybe_str = extism::input().string(5, 2);
    assert(maybe_str);
    assert(*maybe_str == "is");
    auto maybe_vec = extism::input().vec(5, 2);
    assert(maybe_vec);
    assert(std::string(maybe_vec->begin(), maybe_vec->end()) == "is");
    auto maybe_input = extism::input<struct Input>().get_stack(5);
    assert(!maybe_input);
    auto input_ptr = extism::input<struct Input>().get(5);
    assert(!input_ptr);
    auto uintvec = extism::input<uint32_t>().vec(5);
    assert(uintvec);
    assert(uintvec->size() == 2);
  }

  // test loading at end
  {
    auto maybe_str = extism::input().string(14);
    assert(maybe_str);
    assert(!maybe_str->size());
    auto maybe_vec = extism::input().vec(14);
    assert(maybe_vec);
    assert(!maybe_vec->size());
    auto maybe_input = extism::input<struct Input>().get_stack(14);
    assert(!maybe_input);
    auto input_ptr = extism::input<struct Input>().get(14);
    assert(!input_ptr);
  }

  // test loading out of bounds
  {
    auto maybe_str = extism::input().string(15);
    assert(!maybe_str);
    auto maybe_vec = extism::input().vec(15);
    assert(!maybe_vec);
    auto maybe_input = extism::input<struct Input>().get_stack(15);
    assert(!maybe_input);
    auto input_ptr = extism::input<struct Input>().get(15);
    assert(!input_ptr);
  }

  return 0;
}
