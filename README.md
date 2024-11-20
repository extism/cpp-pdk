# Extism C++ PDK

Build [Extism Plug-ins](https://extism.org/docs/concepts/plug-in) in C++.

## Installation

The Extism C++ PDK is a single header library. Copy
[extism-pdk.hpp](https://github.com/extism/cpp-pdk/blob/main/extism-pdk.hpp)
into your project or add this repo as a Git submodule:

```shell
git submodule add https://github.com/extism/cpp-pdk extism-cpp-pdk
```

The [wasi-sdk](https://github.com/WebAssembly/wasi-sdk/releases) is required to
build, extract or install it and point the `WASI_SDK_PATH` environment variable
at it.

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

- `-fno-exceptions` is needed as the Wasm doesn't have support for exceptions
  yet.

- `wasi-sdk-24.0` or later should be used as `-std=c++23` is required by the
  pdk.

- `-mexec-model=reactor` as we're building a
  [reactor](https://dylibso.com/blog/wasi-command-reactor/) module - exporting
  functions, not building a
  [command](https://dylibso.com/blog/wasi-command-reactor/) program.

## Generating Bindings

It's often very useful to define a schema to describe the function signatures
and types you want to use between Extism SDK and PDK languages.

[XTP Bindgen](https://github.com/dylibso/xtp-bindgen) is an open source
framework to generate PDK bindings for Extism plug-ins. It's used by the
[XTP Platform](https://www.getxtp.com/), but can be used outside of the platform
to define any Extism compatible plug-in system.

### 1. Install the `xtp` CLI.

See installation instructions
[here](https://docs.xtp.dylibso.com/docs/cli#installation).

### 2. Create a schema using our OpenAPI-inspired IDL:

```yaml
version: v1-draft
exports: 
  CountVowels:
      input: 
          type: string
          contentType: text/plain; charset=utf-8
      output:
          $ref: "#/components/schemas/VowelReport"
          contentType: application/json
# components.schemas defined in example-schema.yaml...
```

> See an example in [example-schema.yaml](./example-schema.yaml), or a full
> "kitchen sink" example on
> [the docs page](https://docs.xtp.dylibso.com/docs/concepts/xtp-schema/).

### 3. Generate bindings to use from your plugins:

```
xtp plugin init --schema-file ./example-schema.yaml
    1. TypeScript                      
    2. Go                              
    3. Rust                            
    4. Python                          
    5. C#                              
    6. Zig                             
  > 7. C++                             
    8. GitHub Template                 
    9. Local Template
```

This will create an entire boilerplate plugin project for you to get started
with:

```cpp
// returns VowelReport (The result of counting vowels on the Vowels input.)
std::expected<pdk::VowelReport, pdk::Error>
impl::CountVowels(std::string &&input) {
  return std::unexpected(pdk::Error::not_implemented);
}
```

Implement the empty function(s), and run `xtp plugin build` to compile your
plugin.

> For more information about XTP Bindgen, see the
> [dylibso/xtp-bindgen](https://github.com/dylibso/xtp-bindgen) repository and
> the official
> [XTP Schema documentation](https://docs.xtp.dylibso.com/docs/concepts/xtp-schema).

## Reach Out!

Have a question or just want to drop in and say hi?
[Hop on the Discord](https://extism.org/discord)!
