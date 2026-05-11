// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/tests/cpp14_inline_test_tu1.hpp"

#include "arene/base/compiler_support/tests/cpp14_inline_test.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace cpp14_inline_test {
namespace tu1 {

auto example_add_addr() -> std::uintptr_t {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast) Need a reinterpret cast here, and is safe.
  return reinterpret_cast<std::uintptr_t>(&example_add);
}

}  // namespace tu1
}  // namespace cpp14_inline_test
