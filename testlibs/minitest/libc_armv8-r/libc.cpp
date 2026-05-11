// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/libc.hpp"

extern "C" {
[[noreturn]] void abort();
}

namespace testing {
namespace minitest {

[[noreturn]] void abort_program() noexcept { ::abort(); }

[[noreturn]] void exit_program(int status) {
  static_cast<void>(status);
  while (true) {
      // NOLINTNEXTLINE(hicpp-no-assembler)
      asm volatile("bkpt");
  }
}

auto get_env_var(char const* env_var) -> char* {
  static_cast<void>(env_var);
  return {};
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
auto open_file(char const* filename, char const* mode) noexcept -> file_handle {
  static_cast<void>(filename);
  static_cast<void>(mode);
  return {};
}

void close_file(file_handle stream) noexcept { static_cast<void>(stream); }

void flush_file(file_handle stream) noexcept { static_cast<void>(stream); }

auto get_stdout() noexcept -> file_handle { return {}; }
auto get_stderr() noexcept -> file_handle { return {}; }

void print_number(file_handle stream, size_t val) noexcept {
  static_cast<void>(stream);
  static_cast<void>(val);
}
void print_string(file_handle stream, char const* str) noexcept {
  static_cast<void>(stream);
  static_cast<void>(str);
}

}  // namespace minitest
}  // namespace testing

extern "C" {
// Dummy entry point
// NOLINTNEXTLINE(readability-identifier-naming)
void _mcos_hwl_reset() {}
}
