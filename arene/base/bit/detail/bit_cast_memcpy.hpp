// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BIT_DETAIL_BIT_CAST_MEMCPY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BIT_DETAIL_BIT_CAST_MEMCPY_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/bit/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/memcpy.hpp"

namespace arene {
namespace base {
namespace bit_cast_detail {

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

/// @brief Implementation helper for @c arene::base::bit_cast when @c __builtin_bit_cast is not available.
/// @tparam To type to reinterpret as
/// @tparam From type to reinterpret from
/// @param from the source of bits for the return value
/// @return An object of type @c To whose value representation is copied from @c from.
///
/// @note This implementation uses @c std::memcpy and cannot be evaluated in a constant expression.
template <class To, class From>
auto bit_cast(From const& from) noexcept -> To {
  To result;
  ARENE_IGNORE_START();
  // gcc8 flags memcpy on types with user-declared (but defaulted) special members even though they are
  // trivially copyable; the caller has already asserted trivial-copyability via SFINAE constraints.
  ARENE_IGNORE_GCC("-Wclass-memaccess", "Callers constrain both From and To to be trivially copyable");
  std::ignore = std::memcpy(&result, &from, sizeof(To));
  ARENE_IGNORE_END();
  return result;
}

// parasoft-end-suppress AUTOSAR-M2_10_1-a

}  // namespace bit_cast_detail
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BIT_DETAIL_BIT_CAST_MEMCPY_HPP_
