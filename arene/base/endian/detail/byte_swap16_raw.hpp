// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP16_RAW_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP16_RAW_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/endian/.*"

#include "arene/base/endian/detail/byte_swap_constants.hpp"  // IWYU pragma: export
#include "arene/base/stdlib_choice/cstdint.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace byte_swap_detail {

/// @brief Implementation helper for @c arene::base::byte_swap(std::uint16_t) that hand-performs the swap
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
constexpr auto byte_swap_raw(std::uint16_t const value) noexcept -> std::uint16_t {
  return static_cast<std::uint16_t>(
             static_cast<std::uint16_t>(value & byte_swap_detail::low_byte_mask) << byte_swap_detail::bits_in_1_byte
         ) |
         static_cast<std::uint16_t>(value >> byte_swap_detail::bits_in_1_byte);
}

}  // namespace byte_swap_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP16_RAW_HPP_
