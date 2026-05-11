// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP64_BSWAP64_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP64_BSWAP64_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/endian/.*"

#include "arene/base/compiler_support/is_possibly_constant_evaluated.hpp"
#include "arene/base/endian/detail/byte_swap64_raw.hpp"  // IWYU pragma: export
#include "arene/base/stdlib_choice/cstdint.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace byte_swap_detail {

/// @brief Implementation helper for @c arene::base::byte_swap(std::uint64_t) when @c __builtin_bswap64 exists
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
/// This implementation exists because some compilers do not correctly optimize to the builtin, so if we are in a
/// context where we can, we dispatch to it.
constexpr auto byte_swap(std::uint64_t const value) noexcept -> std::uint64_t {
  if (!::arene::base::is_possibly_constant_evaluated()) {
    return __builtin_bswap64(value);
  }
  return ::arene::base::byte_swap_detail::byte_swap_raw(value);
}

}  // namespace byte_swap_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP64_BSWAP64_HPP_
