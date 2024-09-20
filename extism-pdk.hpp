#ifndef extism_pdk_hpp
#define extism_pdk_hpp

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <optional>
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

template <typename T> class Handle : public imports::RawHandle {
  uint64_t byte_size;

public:
  Handle(RawHandle _handle)
      : RawHandle(_handle), byte_size(imports::length(handle)) {}
  Handle(RawHandle handle, uint64_t byte_size)
      : RawHandle(handle), byte_size(byte_size) {}
  static std::optional<Handle<T>> from(std::span<const T> src);
  bool load(std::span<T> dest, const uint64_t src_offset = 0) const;
  std::optional<std::string> string(const uint64_t src_offset,
                                    const size_t max = SIZE_MAX) const;
  std::string string() const { return *string(0); }
  std::optional<std::vector<T>> vec(const uint64_t src_offset,
                                    const size_t max = SIZE_MAX) const;
  std::vector<T> vec() const { return *vec(0); }
  std::optional<T> get_stack(const uint64_t src_offset = 0) const;
  std::unique_ptr<T> get(const uint64_t src_offset = 0) const;
  uint64_t size_bytes() const { return byte_size; }
  bool store(std::span<const T> srca, const uint64_t dest_offset = 0);
};

template <typename T> class UniqueHandle : public Handle<T> {
public:
  explicit UniqueHandle(Handle<T> handle) : Handle<T>(handle) {}
  UniqueHandle(UniqueHandle<T> &&other) : Handle<T>(std::move(other)) {
    other.handle = 0;
  }
  UniqueHandle<T> &operator=(UniqueHandle<T> &&other) {
    if (this != &other) {
      if (this->handle) {
        imports::free(this->handle);
      }
      Handle<T>::operator=(std::move(other));
      other.handle = 0;
    }
    return *this;
  }
  static std::optional<UniqueHandle<T>> from(std::span<const T> src);
  ~UniqueHandle() {
    if (this->handle) {
      imports::free(this->handle);
    }
  }
  Handle<T> release() {
    Handle<T> unowned(std::move(*this));
    this->handle = 0;
    return unowned;
  }
};

template <typename T = char> const Handle<T> input();

bool error_set(const std::string_view s);

template <typename T = char>
std::optional<const UniqueHandle<T>> config(const std::string_view key);

template <typename T = char>
std::optional<const UniqueHandle<T>> var(const std::string_view name);

template <typename T = char>
bool var_set(const std::string_view name, UniqueHandle<T> unique_value);
template <typename T>
bool var_set(const std::string_view name, const std::span<T> value);
bool var_set(const std::string_view name, const std::string_view value);
template <typename T>
bool var_set_type(const std::string_view name, const T &data);

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

template <typename T> void output(UniqueHandle<T> handle);
template <typename T> bool output(std::span<const T> data);
bool output(std::string_view data);
template <typename T> bool output_type(const T &data);

template <typename T> class HttpResponse {
  UniqueHandle<T> handle;

public:
  const int32_t status;
  HttpResponse(UniqueHandle<T> handle)
      : handle(std::move(handle)), status(imports::http_status_code()) {}

  const UniqueHandle<T> &body() { return handle; }
};

template <typename T, typename U>
std::optional<HttpResponse<T>> http_request(const std::string_view req,
                                            const std::span<const U> body = {});

template <typename T>
std::optional<HttpResponse<T>> http_request(const std::string_view req,
                                            const std::string_view body = "");

// template implementation
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
bool Handle<T>::store(std::span<const T> src_span, const uint64_t dest_offset) {
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
std::optional<std::vector<T>> Handle<T>::vec(const uint64_t src_offset,
                                             const size_t max_bytes) const {
  if (src_offset > byte_size) {
    return std::nullopt;
  }
  const uint64_t bufsize =
      std::min<uint64_t>(byte_size - src_offset, max_bytes);
  std::vector<T> vec(bufsize / sizeof(T));
  if (!load(vec, src_offset)) {
    return std::nullopt;
  }
  return vec;
}

template <typename T>
std::optional<std::string> Handle<T>::string(const uint64_t src_offset,
                                             const size_t max) const {
  if (src_offset > byte_size) {
    return std::nullopt;
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

template <typename T>
std::optional<T> Handle<T>::get_stack(const uint64_t src_offset) const {
  T t;
  if (!load(std::span<T, 1>{std::addressof(t), 1}, src_offset)) {
    return std::nullopt;
  }
  return t;
}

template <typename T>
std::unique_ptr<T> Handle<T>::get(const uint64_t src_offset) const {
  auto ptr = std::make_unique<T>();
  if (!load(std::span<T, 1>{ptr.get(), 1}, src_offset)) {
    return nullptr;
  }
  return ptr;
}

template <typename T>
std::optional<Handle<T>> Handle<T>::from(std::span<const T> src) {
  auto handle = imports::alloc(src.size_bytes());
  if (!handle && src.size_bytes()) {
    return std::nullopt;
  }
  Handle<T> h(handle, src.size_bytes());
  if (!h.store(src)) {
    imports::free(h);
    return std::nullopt;
  }
  return h;
}

template <typename T>
std::optional<UniqueHandle<T>> UniqueHandle<T>::from(std::span<const T> src) {
  if (auto h = Handle<T>::from(src)) {
    return UniqueHandle<T>(*h);
  }
  return std::nullopt;
}

template <typename T> const Handle<T> input() {
  return Handle<T>(imports::input());
}

template <typename T> void output(UniqueHandle<T> unique_handle) {
  auto handle = unique_handle.release();
  imports::output_set(handle, handle.size_bytes());
}

template <typename T> bool output(std::span<const T> data) {
  if (auto h = Handle<const T>::from(data)) {
    imports::output_set(*h, data.size_bytes());
    return true;
  }
  return false;
}

template <typename T> bool output_type(const T &data) {
  if (auto h = Handle<const T>::from(
          std::span<const T, 1>{std::addressof(data), 1})) {
    imports::output_set(*h, sizeof(data));
    return true;
  }
  return false;
}

template <typename T>
std::optional<const UniqueHandle<T>> config(const std::string_view key) {
  if (auto kh = Handle<char>::from(key)) {
    auto rawvh = imports::config_get(*kh);
    if (rawvh) {
      return UniqueHandle<T>(rawvh);
    }
  }
  return std::nullopt;
}

template <typename T>
std::optional<const UniqueHandle<T>> var(const std::string_view name) {
  if (auto kh = Handle<char>::from(name)) {
    auto rawvh = imports::var_get(*kh);
    if (rawvh) {
      return UniqueHandle<T>(rawvh);
    }
  }
  return std::nullopt;
}

template <typename T>
bool var_set(const std::string_view name, UniqueHandle<T> unique_value) {
  auto nh = Handle<const char>::from(name);
  if (!nh) {
    return false;
  }
  auto value = unique_value.release();
  imports::var_set(*nh, value);
  return true;
}

template <typename T>
bool var_set(const std::string_view name, const std::span<T> value) {
  auto nh = Handle<const char>::from(name);
  if (!nh) {
    return false;
  }
  auto vh = Handle<T>::from(value);
  if (!vh) {
    return false;
  }
  imports::var_set(*nh, *vh);
  return true;
}

template <typename T>
bool var_set_type(const std::string_view name, const T &data) {
  auto nh = Handle<const char>::from(name);
  if (!nh) {
    return false;
  }
  if (auto h = Handle<const T>::from(
          std::span<const T, 1>{std::addressof(data), 1})) {
    imports::var_set(*nh, *h);
    return true;
  }
  return false;
}

template <typename T, typename U>
std::optional<HttpResponse<T>> http_request(const std::string_view req,
                                            const std::span<const U> body) {
  auto reqh = Handle<char>::from(req);
  if (!reqh) {
    return std::nullopt;
  }
  auto bh = Handle<U>::from(body);
  if (!bh) {
    return std::nullopt;
  }
  auto rawh = imports::http_request(*reqh, *bh);
  if (!rawh) {
    return std::nullopt;
  }
  return HttpResponse<T>(UniqueHandle<T>(rawh));
}

template <typename T>
std::optional<HttpResponse<T>> http_request(const std::string_view req,
                                            const std::string_view body) {
  return http_request<T, const char>(req, std::span{body});
}

// end template implementation
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

bool output(std::string_view data) { return output<const char>(data); }

bool error_set(const std::string_view s) {
  if (auto h = Handle<const char>::from(s)) {
    imports::error_set(*h);
    return true;
  }
  return false;
}

bool var_set(const std::string_view name, const std::string_view value) {
  return var_set<const char>(name, value);
}

// Write to Extism log
bool log(const std::string_view message, const Log level) {
  auto buf = Handle<const char>::from(message);
  if (!buf) {
    return false;
  }
  switch (level) {
  case Info:
    imports::log_info(*buf);
    break;
  case Debug:
    imports::log_debug(*buf);
    break;
  case Warn:
    imports::log_warn(*buf);
    break;
  case Error:
    imports::log_error(*buf);
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
