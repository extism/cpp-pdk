# Extism C++ PDK

Build [Extism Plug-ins](https://extism.org/docs/concepts/plug-in) in C++.

## Installation

The Extism C++ PDK is a single header library. Copy [extism-pdk.hpp](https://github.com/extism/cpp-pdk/blob/main/extism-pdk.hpp) into your project or add this repo as a Git submodule:

```shell
git submodule add https://github.com/extism/cpp-pdk extism-cpp-pdk
```

The [wasi-sdk](https://github.com/WebAssembly/wasi-sdk/releases) is required to build, extract or install it and point the `WASI_SDK_PATH` environment variable at it.

## Getting Started

TODO

## Building

As the C++ pdk is implemented as single header library, in **ONE** source file:

```c++
#define EXTISM_CPP_IMPLEMENTATION
#include "extism-pdk.hpp"
```

In other source files, just `#include "extism-pdk.hpp"`

Compile:

`$(WASI_SDK_PATH)/bin/clang++ -std=c++23 -fno-exceptions -O2 -g -o count-vowels.wasm count-vowels.cpp -mexec-model=reactor`

* `-fno-exceptions` is needed as the Wasm doesn't have support for exceptions yet.

*  `wasi-sdk-24.0` or later should be used as `-std=c++23` is required by the pdk.

* `-mexec-model=reactor` as we're building a [reactor](https://dylibso.com/blog/wasi-command-reactor/) module - exporting functions, not building a [command](https://dylibso.com/blog/wasi-command-reactor/) program.

## Exports (details)

The `EXTISM_EXPORTED_FUNCTION` macro is not essential to create a plugin function and export it to the host. You may instead write a function and then export it when linking. For example, the first example may have the following signature instead:

```c
int32_t greet(void)
```

Then, it can be built and linked with:

```bash
$WASI_SDK_PATH/bin/clang++ -o plugin.wasm --target=wasm32-wasi -Wl,--no-entry -Wl,--export=greet plugin.cpp
```

Note the `-Wl,--export=greet`

Exports names do not necessarily have to match the function name either. Going back to the first example again. Try:

```c
EXTISM_EXPORT_AS("greet") int32_t internal_name_for_greet(void)
```

and build with:

```bash
$WASI_SDK_PATH/bin/clang++ -o plugin.wasm --target=wasm32-wasi -Wl,--no-entry plugin.cpp
```

## Reach Out!

Have a question or just want to drop in and say hi? [Hop on the Discord](https://extism.org/discord)!
