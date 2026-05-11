// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP_CONSTANTS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP_CONSTANTS_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/endian/.*"

#include "arene/base/stdlib_choice/cstdint.hpp"

namespace arene {
namespace base {
namespace byte_swap_detail {
/// @brief The number of bits in 1 byte
constexpr std::uint32_t bits_in_1_byte{8U};
/// @brief The number of bits in 3 bytes
constexpr std::uint32_t bits_in_3_bytes{24U};
/// @brief The number of bits in 5 bytes
constexpr std::uint32_t bits_in_5_bytes{40U};
/// @brief The number of bits in 7 bytes
constexpr std::uint32_t bits_in_7_bytes{56U};

/// @brief A bitmask for the bits in the lowest byte
constexpr std::uint16_t low_byte_mask{0xFFU};
/// @brief A bitmask for the bits in the second lowest byte
constexpr std::uint32_t second_byte_mask{0xFF00U};
/// @brief A bitmask for the bits in the third lowest byte
constexpr std::uint64_t third_byte_mask{0xFF0000U};
/// @brief A bitmask for the bits in the fourth lowest byte
constexpr std::uint64_t fourth_byte_mask{0xFF000000U};
}  // namespace byte_swap_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_DETAIL_BYTE_SWAP_CONSTANTS_HPP_
