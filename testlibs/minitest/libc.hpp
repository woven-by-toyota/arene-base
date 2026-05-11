// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_LIBC_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_LIBC_HPP_

#include <stddef.h>  // NOLINT(hicpp-deprecated-headers) This is used to test a standard library so it can't use std

namespace testing {
namespace minitest {
[[noreturn]] void abort_program() noexcept;
[[noreturn]] void exit_program(int status) noexcept;
auto get_env_var(char const* env_var) noexcept -> char*;

class file_handle {
  void* os_handle_ = nullptr;

 public:
  file_handle() = default;
  explicit file_handle(void* os_handle) noexcept
      : os_handle_(os_handle) {}

  auto get_handle() const noexcept -> void* { return os_handle_; }

  explicit operator bool() const noexcept { return os_handle_ != nullptr; }
};

auto open_file(char const* filename, char const* mode) noexcept -> file_handle;
void close_file(file_handle stream) noexcept;
void flush_file(file_handle stream) noexcept;
auto get_stdout() noexcept -> file_handle;
auto get_stderr() noexcept -> file_handle;

void print_number(file_handle stream, size_t val) noexcept;
inline void print_number(size_t val) noexcept { print_number(get_stdout(), val); }
void print_string(file_handle stream, char const* str) noexcept;
inline void print_string(char const* str) noexcept { print_string(get_stdout(), str); }

}  // namespace minitest
}  // namespace testing

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_MINITEST_LIBC_HPP_
