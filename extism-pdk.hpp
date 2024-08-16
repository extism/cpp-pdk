#ifndef extism_pdk_hpp
#define extism_pdk_hpp

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
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

template <typename T>
std::vector<T> input_vec(const uint64_t src_offset = 0,
                         const size_t max = SIZE_MAX);
std::string input_string(const uint64_t src_offset = 0,
                         const size_t max = SIZE_MAX);
template <typename T> T input_stack(const uint64_t src_offset = 0);
template <typename T> std::unique_ptr<T> input(const uint64_t src_offset = 0);

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

bool log_info(const std::string_view message);
bool log_debug(const std::string_view message);
bool log_warn(const std::string_view message);
bool log_error(const std::string_view message);

typedef enum {
  Info,
  Debug,
  Warn,
  Error,
} Log;

bool log(const std::string_view message, const Log level);

template <typename T> bool output(std::span<const T> data);
bool output(std::string_view data);
template <typename T> bool output_type(T &data);

template <typename T> class Handle : public imports::RawHandle {
  uint64_t byte_size;

public:
  Handle(RawHandle _handle)
      : RawHandle(_handle), byte_size(imports::length(handle)) {}
  Handle(RawHandle handle, uint64_t byte_size)
      : RawHandle(handle), byte_size(byte_size) {}
  bool load(std::span<T> dest, const uint64_t src_offset = 0) const;
  bool store(std::span<const T> srca, const uint64_t dest_offset = 0) const;
  std::string string(const uint64_t src_offset = 0,
                     const size_t max = SIZE_MAX) const;
  std::vector<T> vec(const uint64_t src_offset = 0,
                     const size_t max = SIZE_MAX) const;
  T get_stack(const uint64_t src_offset = 0);
  std::unique_ptr<T> get(const uint64_t src_offset = 0);
};

template <typename T> class OwnedHandle : public Handle<T> {
public:
  OwnedHandle(Handle<T> handle) : Handle<T>(handle) {}
  ~OwnedHandle() { imports::free(this->handle); }
};

template <typename T> class HttpResponse {
  Handle<std::byte> handle;

public:
  const int32_t status;
  HttpResponse(Handle<T> handle)
      : handle(handle), status(imports::http_status_code()) {}

  std::vector<T> body_vec() const;
  std::string body_string() const;
};

template <typename T, typename U>
HttpResponse<T> http_request(const std::string_view req,
                             const std::span<U> body);

template <typename T>
HttpResponse<T> http_request(const std::string_view req,
                             const std::string_view body);
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

template <typename T>
bool Handle<T>::load(std::span<T> dest_span, const uint64_t src_offset) const {
  if (src_offset + dest_span.size_bytes() > byte_size) {
    return false;
  }

  auto dest = std::as_writable_bytes(dest_span);
  const uint64_t offs = handle + src_offset;
  auto it = dest.begin();
  const auto chunkend = dest.begin() + (dest.size() / 8) * 8;
  for (; it != chunkend; it += 8) {
    const uint64_t value = imports::load_u64(offs + (it - dest.begin()));
    std::memcpy(std::to_address(it), &value, sizeof(value));
  }
  for (; it != dest.end(); ++it) {
    *it = static_cast<std::byte>(imports::load_u8(offs + (it - dest.begin())));
  }
  return true;
}

template <typename T>
bool Handle<T>::store(std::span<const T> src_span,
                      const uint64_t dest_offset) const {
  if (dest_offset + src_span.size_bytes() > byte_size) {
    return false;
  }

  auto byte_src = std::as_bytes(src_span);
  const uint64_t offs = handle + dest_offset;
  auto it = byte_src.begin();
  const auto chunkend = byte_src.begin() + (byte_src.size() / 8) * 8;
  for (; it != chunkend; it += 8) {
    uint64_t value;
    std::memcpy(&value, std::to_address(it), sizeof(value));
    imports::store_u64(offs + (it - byte_src.begin()), value);
  }
  for (; it != byte_src.end(); ++it) {
    imports::store_u8(offs + (it - byte_src.begin()),
                      static_cast<uint8_t>(*it));
  }
  return true;
}

template <typename T>
std::vector<T> Handle<T>::vec(const uint64_t src_offset,
                              const size_t max_bytes) const {
  if (src_offset >= byte_size) {
    return {};
  }
  const uint64_t bufsize =
      std::min<uint64_t>(byte_size - src_offset, max_bytes);
  std::vector<T> vec(bufsize / sizeof(T));
  load(vec, src_offset);
  return vec;
}

template <typename T>
std::string Handle<T>::string(const uint64_t src_offset,
                              const size_t max) const {
  if (src_offset >= byte_size) {
    return "";
  }
  std::string output;
  const uint64_t bufsize = std::min<uint64_t>(byte_size - src_offset, max);
  output.resize_and_overwrite(bufsize,
                              [this, src_offset](char *buf, size_t n) noexcept {
                                load({buf, n}, src_offset);
                                return n;
                              });
  return output;
}

template <typename T> T Handle<T>::get_stack(const uint64_t src_offset) {
  if ((src_offset + sizeof(T)) > byte_size) {
    return {};
  }
  T t;
  load(std::span<T, 1>{std::addressof(t), 1}, src_offset);
  return t;
}

template <typename T>
std::unique_ptr<T> Handle<T>::get(const uint64_t src_offset) {
  if ((src_offset + sizeof(T)) > byte_size) {
    return nullptr;
  }
  auto ptr = std::make_unique<T>();
  load(std::span<T, 1>{ptr.get(), 1}, src_offset);
  return ptr;
}

template <typename T>
std::vector<T> input_vec(const uint64_t src_offset, const size_t max) {
  Handle<T> handle(imports::input());
  return handle.vec(src_offset, max);
}

std::string input_string(const uint64_t src_offset, const size_t max) {
  Handle<char> handle(imports::input());
  return handle.string(src_offset, max);
}

template <typename T> T input_stack(const uint64_t src_offset) {
  Handle<T> handle(imports::input());
  return handle.get_stack();
}

template <typename T> std::unique_ptr<T> input(const uint64_t src_offset) {
  Handle<T> handle(imports::input());
  return handle.get();
}

template <typename T> bool output(std::span<const T> data) {
  auto handle = imports::alloc(data.size_bytes());
  if (!handle) {
    return false;
  }
  Handle<T> h(handle, data.size_bytes());
  if (!h.store(data)) {
    imports::free(h);
    return false;
  }
  imports::output_set(h, data.size_bytes());
  return true;
}

bool output(std::string_view data) { return output<const char>(data); }

template <typename T> bool output_type(T &data) {
  auto handle = imports::alloc(sizeof(data));
  if (!handle) {
    return false;
  }
  Handle<T> h(handle, sizeof(data));
  if (!h.store(std::span<T, 1>{std::addressof(data), 1})) {
    imports::free(h);
    return false;
  }
  imports::output_set(h, sizeof(data));
  return true;
}

// Write to Extism log
bool log(const std::string_view message, const Log level) {
  auto rawbuf = imports::alloc(message.length());
  if (!rawbuf) {
    return false;
  }
  OwnedHandle<const char> buf(rawbuf);
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

bool log_info(const std::string_view message) { return log(message, Info); }
bool log_debug(const std::string_view message) { return log(message, Debug); }

bool log_warn(const std::string_view message) { return log(message, Warn); }
bool log_error(const std::string_view message) { return log(message, Error); }

} // namespace extism

#endif // extism_pdk_cpp
#endif // EXTISM_CPP_IMPLEMENTATION
