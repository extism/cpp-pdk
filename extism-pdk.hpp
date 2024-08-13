#ifndef extism_pdk_hpp
#define extism_pdk_hpp

#include <algorithm>
#include <cstddef>
#include <span>
#include <stdint.h>
#include <string>
#include <string_view>
#include <vector>

namespace extism {
#define EXTISM_ENV_MODULE "extism:host/env"
#define EXTISM_USER_MODULE "extism:host/user"

#define EXTISM_EXPORT_AS(name) __attribute__((export_name(name)))
#define EXTISM_EXPORTED_FUNCTION(name)                                         \
  EXTISM_EXPORT_AS(#name)                                                      \
  name(void)

#define EXTISM_IMPORT(a, b) __attribute__((import_module(a), import_name(b)))
#define EXTISM_IMPORT_ENV(b)                                                   \
  __attribute__((import_module(EXTISM_ENV_MODULE), import_name(b)))
#define EXTISM_IMPORT_USER(b)                                                  \
  __attribute__((import_module(EXTISM_USER_MODULE), import_name(b)))

namespace imports {
typedef uint64_t ExtismPointer;
class RawHandle {
protected:
  uint64_t handle;

public:
  RawHandle(uint64_t _handle) : handle(_handle) {};
  explicit operator bool() const { return handle; }
  operator ExtismPointer() const { return handle; }
};
static_assert(sizeof(RawHandle) == sizeof(uint64_t));

EXTISM_IMPORT_ENV("length_unsafe") extern uint64_t length(const RawHandle);

EXTISM_IMPORT_ENV("input_offset")
extern RawHandle input(void);

EXTISM_IMPORT_ENV("alloc")
extern RawHandle alloc(const uint64_t);
EXTISM_IMPORT_ENV("free")
extern void free(RawHandle);

EXTISM_IMPORT_ENV("error_set")
extern void error_set(const RawHandle);

EXTISM_IMPORT_ENV("config_get")
extern RawHandle config_get(const RawHandle);

EXTISM_IMPORT_ENV("var_get")
extern RawHandle var_get(const RawHandle);

EXTISM_IMPORT_ENV("var_set")
extern void var_set(RawHandle, const RawHandle);

EXTISM_IMPORT_ENV("http_request")
extern RawHandle http_request(const RawHandle, const RawHandle);

EXTISM_IMPORT_ENV("http_status_code")
extern int32_t http_status_code(void);

EXTISM_IMPORT_ENV("log_info")
extern void log_info(const RawHandle);
EXTISM_IMPORT_ENV("log_debug")
extern void log_debug(const RawHandle);
EXTISM_IMPORT_ENV("log_warn")
extern void log_warn(const RawHandle);
EXTISM_IMPORT_ENV("log_error")
extern void log_error(const RawHandle);

EXTISM_IMPORT_ENV("output_set")
extern void output_set(const ExtismPointer, const uint64_t);
EXTISM_IMPORT_ENV("store_u8")
extern void store_u8(ExtismPointer, const uint8_t);
EXTISM_IMPORT_ENV("load_u8")
extern uint8_t load_u8(const ExtismPointer);
EXTISM_IMPORT_ENV("store_u64")
extern void store_u64(ExtismPointer, const uint64_t);
EXTISM_IMPORT_ENV("load_u64")
extern uint64_t load_u64(const ExtismPointer);
}; // namespace imports

std::vector<std::byte> input_bytes(const uint64_t src_offset = 0,
                                   const size_t max = SIZE_MAX);
std::string input_string(const uint64_t src_offset = 0,
                         const size_t max = SIZE_MAX);

void error_set(const std::string_view s);

std::string config_string(const std::string_view key, const uint64_t offset = 0,
                          const size_t max = SIZE_MAX);

std::vector<std::byte> var_bytes(const std::string_view name,
                                 const uint64_t offset = 0,
                                 const size_t max = SIZE_MAX);

std::string var_string(const std::string_view name, const uint64_t offset = 0,
                       const size_t max = SIZE_MAX);

void var_set(const std::string_view name, const std::string_view value);

void var_set(const std::string_view name, const std::span<std::byte> value);

void log_info(const std::string_view message);
void log_debug(const std::string_view message);
void log_warn(const std::string_view message);
void log_error(const std::string_view message);

typedef enum {
  Info,
  Debug,
  Warn,
  Error,
} Log;

bool log(const std::string_view message, const Log level);

bool output(const std::string_view data);
bool output(const std::span<std::byte> data);

class Handle : public imports::RawHandle {
  uint64_t size;

public:
  Handle(RawHandle _handle)
      : RawHandle(_handle), size(imports::length(handle)) {}
  Handle(RawHandle handle, uint64_t size) : RawHandle(handle), size(size) {}
  std::string string(const uint64_t src_offset = 0,
                     const size_t max = SIZE_MAX) const;
  std::vector<std::byte> bytes(const uint64_t src_offset = 0,
                               const size_t max = SIZE_MAX) const;
  bool load(std::span<std::byte> dest, const uint64_t src_offset = 0) const;
  bool store(std::span<std::byte> src, const uint64_t dest_offset = 0);
  bool store(std::string_view src, const uint64_t dest_offset = 0);
};

class OwnedHandle : public Handle {
public:
  OwnedHandle(Handle handle) : Handle(handle) {}
  ~OwnedHandle() { imports::free(handle); }
};

class HttpResponse {
  Handle handle;

public:
  const int32_t status;
  HttpResponse(Handle handle)
      : handle(handle), status(imports::http_status_code()) {}

  std::vector<std::byte> body_bytes() const;
  std::string body_string() const;
};

HttpResponse http_request(const std::string_view req,
                          const std::string_view body);
HttpResponse http_request(const std::string_view req,
                          const std::span<std::byte> body);

} // namespace extism

#endif // extism_pdk_hpp

// avoid greying out the implementation section
#if defined(Q_CREATOR_RUN) || defined(__INTELLISENSE__) ||                     \
    defined(_CDT_PARSER__)
#define EXTISM_CPP_IMPLEMENTATION
#endif

#ifdef EXTISM_CPP_IMPLEMENTATION
#ifndef extism_pdk_cpp
#define extism_pdk_cpp

namespace extism {

bool Handle::load(std::span<std::byte> dest, const uint64_t src_offset) const {
  if (src_offset + dest.size() > size) {
    return false;
  }
  const uint64_t offs = handle + src_offset;
  const size_t n = dest.size();
  const size_t chunk_count = n >> 3;
  uint64_t *i64_buffer = reinterpret_cast<uint64_t *>(dest.data());
  for (size_t chunk_idx = 0; chunk_idx < chunk_count; chunk_idx++) {
    i64_buffer[chunk_idx] = imports::load_u64(offs + (chunk_idx << 3));
  }

  size_t remainder_offset = chunk_count << 3;
  const size_t remainder_end = remainder_offset + (n & 7);
  for (uint8_t *u8_buffer = reinterpret_cast<uint8_t *>(dest.data());
       remainder_offset < remainder_end; remainder_offset++) {
    u8_buffer[remainder_offset] = imports::load_u8(offs + remainder_offset);
  }
  return true;
}

bool Handle::store(std::string_view src, const uint64_t dest_offset) {
  if (dest_offset + src.size() > size) {
    return false;
  }
  const size_t length = src.size();
  const size_t chunk_count = length >> 3;
  const uint64_t offs = handle + dest_offset;
  const uint64_t *i64_buffer = reinterpret_cast<const uint64_t *>(src.data());
  for (size_t chunk_idx = 0; chunk_idx < chunk_count; chunk_idx++) {
    imports::store_u64(offs + (chunk_idx << 3), i64_buffer[chunk_idx]);
  }

  size_t remainder_offset = chunk_count << 3;
  const size_t remainder_end = remainder_offset + (length & 7);
  for (const uint8_t *u8_buffer = reinterpret_cast<const uint8_t *>(src.data());
       remainder_offset < remainder_end; remainder_offset++) {
    imports::store_u8(offs + remainder_offset, u8_buffer[remainder_offset]);
  }
  return true;
}

std::string Handle::string(const uint64_t src_offset, const size_t max) const {
  if (src_offset >= size) {
    return "";
  }
  std::string output;
  const uint64_t bufsize = std::min<uint64_t>(size - src_offset, max);
  output.resize_and_overwrite(bufsize,
                              [this, src_offset](char *buf, size_t n) noexcept {
                                std::span myspan{(std::byte *)buf, n};
                                load(myspan, src_offset);
                                return n;
                              });
  return output;
}

std::string input_string(const uint64_t src_offset, const size_t max) {
  Handle handle(imports::input());
  return handle.string(src_offset, max);
}

bool output(const std::string_view data) {
  auto handle = imports::alloc(data.size());
  if (!handle) {
    return false;
  }
  Handle h(std::move(handle), data.size());
  if (!h.store(data)) {
    imports::free(h);
    return false;
  }
  imports::output_set(h, data.size());
  return true;
}

// Write to Extism log
bool log(const std::string_view message, const Log level) {
  auto rawbuf = imports::alloc(message.length());
  if (!rawbuf) {
    return false;
  }
  OwnedHandle buf(std::move(rawbuf));
  if (!buf.store(message)) {
    return false;
  }
  switch (level) {
  case Info:
    imports::log_info(buf);
    break;
  case Debug:
    imports::log_debug(buf);
    break;
  case Warn:
    imports::log_warn(buf);
    break;
  case Error:
    imports::log_error(buf);
    break;
  }
  return true;
}
} // namespace extism

#endif // extism_pdk_cpp
#endif // EXTISM_CPP_IMPLEMENTATION
