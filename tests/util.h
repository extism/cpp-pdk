#pragma once

#define assert(x)                                                              \
  if (!(x)) {                                                                  \
    return 1;                                                                  \
  }
