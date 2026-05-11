// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

///
/// @example examples/subpackages/bit_examples.cpp
/// @brief Examples for the bit subpackage documentation
///

//! [include_header]
#include "arene/base/bit.hpp"
//! [include_header]

#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/compiler_support/preprocessor.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"

namespace {

//! [basic_usage]
/// @brief Reinterpret the object representation of a @c float as the matching
/// IEEE 754 unsigned integer bit pattern.
auto float_to_bits(float const value) noexcept -> std::uint32_t { return arene::base::bit_cast<std::uint32_t>(value); }
//! [basic_usage]

//! [constexpr_availability]
#if ARENE_IS_ON(ARENE_HAS_CONSTEXPR_BIT_CAST)
/// @brief When @c __builtin_bit_cast is available, @c bit_cast can be used
/// inside a @c constexpr context.
constexpr auto one_f_bits = arene::base::bit_cast<std::uint32_t>(1.0F);
// NOLINTNEXTLINE(readability-magic-numbers) IEEE 754 single-precision bit pattern for 1.0F
static_assert(one_f_bits == 0x3F800000U, "IEEE 754 single-precision bit pattern for 1.0F");
#endif
//! [constexpr_availability]

}  // namespace

auto main() -> int {
  (void)float_to_bits(1.0F);
  return 0;
}
