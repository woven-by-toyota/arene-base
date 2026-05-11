// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP32_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP32_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/endian/.*"

#include "arene/base/endian/detail/byte_swap32_raw.hpp"  // IWYU pragma: export
#include "arene/base/stdlib_choice/cstdint.hpp"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax pmeritted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {
namespace byte_swap_detail {

/// @brief Implementation helper for @c arene::base::byte_swap(std::uint32_t) when @c __builtin_bswap32 does not exist
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
constexpr auto byte_swap(std::uint32_t const value) noexcept -> std::uint32_t { return byte_swap_raw(value); }

}  // namespace byte_swap_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP32_HPP_
