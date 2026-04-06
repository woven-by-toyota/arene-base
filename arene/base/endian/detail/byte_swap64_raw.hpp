// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP64_RAW_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP64_RAW_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/endian/.*"

#include "arene/base/endian/detail/byte_swap_constants.hpp"  // IWYU pragma: export
#include "arene/base/stdlib_choice/cstdint.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace byte_swap_detail {

/// @brief Implementation helper for @c arene::base::byte_swap(std::uint64_t) that hand-rolls the swap
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
constexpr auto byte_swap_raw(std::uint64_t const value) noexcept -> std::uint64_t {
  return (value >> byte_swap_detail::bits_in_7_bytes) |
         ((value >> byte_swap_detail::bits_in_5_bytes) & static_cast<std::uint64_t>(byte_swap_detail::second_byte_mask)
         ) |
         ((value >> byte_swap_detail::bits_in_3_bytes) & byte_swap_detail::third_byte_mask) |
         ((value >> byte_swap_detail::bits_in_1_byte) & byte_swap_detail::fourth_byte_mask) |
         ((value & byte_swap_detail::fourth_byte_mask) << byte_swap_detail::bits_in_1_byte) |
         ((value & byte_swap_detail::third_byte_mask) << byte_swap_detail::bits_in_3_bytes) |
         ((value & static_cast<std::uint64_t>(byte_swap_detail::second_byte_mask))
          << byte_swap_detail::bits_in_5_bytes) |
         (value << byte_swap_detail::bits_in_7_bytes);
}

}  // namespace byte_swap_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP64_RAW_HPP_
