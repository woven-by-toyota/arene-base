// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/stdlib_choice/stdexcept.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {
namespace inline_map_detail {

// parasoft-begin-suppress AUTOSAR-A3_3_1-a "False positive: This function is declared in the map.hpp header."
/// @brief Implementation helper for conditional disablement of throwing when compiling with @c -fno-exceptions .
/// @throws std::out_of_range unconditionally.
ARENE_NORETURN void throw_out_of_range() { throw std::out_of_range("inline_map: out of range"); }
// parasoft-end-suppress AUTOSAR-A3_3_1-a

}  // namespace inline_map_detail
}  // namespace base
}  // namespace arene
