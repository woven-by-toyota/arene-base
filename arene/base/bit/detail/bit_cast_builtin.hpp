// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BIT_DETAIL_BIT_CAST_BUILTIN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BIT_DETAIL_BIT_CAST_BUILTIN_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/bit/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace bit_cast_detail {

/// @brief Implementation helper for @c arene::base::bit_cast when @c __builtin_bit_cast is available.
/// @tparam To type to reinterpret as
/// @tparam From type to reinterpret from
/// @param from the source of bits for the return value
/// @return An object of type @c To whose value representation is copied from @c from.
template <class To, class From>
constexpr auto bit_cast(From const& from) noexcept -> To {
  return __builtin_bit_cast(To, from);
}

}  // namespace bit_cast_detail
}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-A7_1_5-a-2

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BIT_DETAIL_BIT_CAST_BUILTIN_HPP_
