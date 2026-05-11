// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "testlibs/minitest/libc.hpp"

// NOLINTBEGIN(hicpp-deprecated-headers)
#include <stddef.h>
#include <stdio.h>
#include <string.h>
// NOLINTEND(hicpp-deprecated-headers)

#if !defined(__clang__) && defined(__GNUC__)
// NOTE: we *cannot* include stdlib.h with GCC toolchains from
// '@arene-toolchains'. 'include/c++/...' is included in builtin toolchain
// include directories, which results in a C++ version of 'stdlib.h' getting
// included. This file then includes 'cstdlib' which picks up our dummy version
// -- causing a build error.
extern "C" {
[[noreturn]] void abort();
[[noreturn]] void exit(int status);
auto getenv(char const*) -> char*;
}
#else
// NOLINTNEXTLINE(hicpp-deprecated-headers)
#include <stdlib.h>
#endif

namespace testing {
namespace minitest {

[[noreturn]] void abort_program() noexcept { ::abort(); }
[[noreturn]] void exit_program(int status) noexcept {
  ::exit(status);  // NOLINT(concurrency-mt-unsafe)
}
auto get_env_var(char const* env_var) noexcept -> char* {
  return ::getenv(env_var);  // NOLINT(concurrency-mt-unsafe)
}

auto open_file(char const* filename, char const* mode) noexcept -> file_handle {
  return file_handle{::fopen(filename, mode)};
}
void close_file(file_handle stream) noexcept { static_cast<void>(::fclose(static_cast<FILE*>(stream.get_handle()))); }
void flush_file(file_handle stream) noexcept { static_cast<void>(::fflush(static_cast<FILE*>(stream.get_handle()))); }

auto get_stdout() noexcept -> file_handle { return file_handle{stdout}; }
auto get_stderr() noexcept -> file_handle { return file_handle{stderr}; }

void print_number(file_handle stream, size_t val) noexcept {
  // NOLINTNEXTLINE(hicpp-vararg)
  fprintf(static_cast<FILE*>(stream.get_handle()), "%zu", val);
}
void print_string(file_handle stream, char const* str) noexcept {
  // NOLINTNEXTLINE(hicpp-vararg)
  fprintf(static_cast<FILE*>(stream.get_handle()), "%s", str);
}
}  // namespace minitest
}  // namespace testing
