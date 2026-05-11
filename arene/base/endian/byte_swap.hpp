// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_BYTE_SWAP_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_BYTE_SWAP_HPP_

// IWYU pragma: private, include "arene/base/endian.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax pmeritted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/platform_queries.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-M16_0_7-a-2 "False positive: Not used as macro"
#if ARENE_HAS_BUILTIN(__builtin_bswap16)
#include "arene/base/endian/detail/byte_swap16_bswap16.hpp"  // IWYU pragma: export
#else
#include "arene/base/endian/detail/byte_swap16.hpp"  // IWYU pragma: export
#endif
#if ARENE_HAS_BUILTIN(__builtin_bswap32)
#include "arene/base/endian/detail/byte_swap32_bswap32.hpp"  // IWYU pragma: export
#else
#include "arene/base/endian/detail/byte_swap32.hpp"  // IWYU pragma: export
#endif
#if ARENE_HAS_BUILTIN(__builtin_bswap64)
#include "arene/base/endian/detail/byte_swap64_bswap64.hpp"  // IWYU pragma: export
#else
#include "arene/base/endian/detail/byte_swap64.hpp"  // IWYU pragma: export
#endif
// parasoft-end-suppress AUTOSAR-M16_0_7-a-2

namespace arene {
namespace base {

/// @brief Swap the order of bytes in a given value.
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
constexpr auto byte_swap(std::uint8_t const value) noexcept -> std::uint8_t { return value; }
/// @brief Swap the order of bytes in a given value.
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
constexpr auto byte_swap(std::uint16_t const value) noexcept -> std::uint16_t {
  return byte_swap_detail::byte_swap(value);
}
/// @brief Swap the order of bytes in a given value.
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
constexpr auto byte_swap(std::uint32_t const value) noexcept -> std::uint32_t {
  return byte_swap_detail::byte_swap(value);
}
/// @brief Swap the order of bytes in a given value.
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
constexpr auto byte_swap(std::uint64_t const value) noexcept -> std::uint64_t {
  return byte_swap_detail::byte_swap(value);
}
/// @brief Swap the order of bytes in a given value.
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
constexpr auto byte_swap(std::int8_t const value) noexcept -> std::int8_t { return value; }
/// @brief Swap the order of bytes in a given value.
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
constexpr auto byte_swap(std::int16_t const value) noexcept -> std::int16_t {
  return static_cast<std::int16_t>(::arene::base::byte_swap(static_cast<std::uint16_t>(value)));
}

/// @brief Swap the order of bytes in a given value.
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
constexpr auto byte_swap(std::int32_t const value) noexcept -> std::int32_t {
  return static_cast<std::int32_t>(::arene::base::byte_swap(static_cast<std::uint32_t>(value)));
}

/// @brief Swap the order of bytes in a given value.
/// @param value The value to byte-swap.
/// @return A value having the representation that has the same bytes as @c value but with the byte-order reversed.
constexpr auto byte_swap(std::int64_t const value) noexcept -> std::int64_t {
  return static_cast<std::int64_t>(::arene::base::byte_swap(static_cast<std::uint64_t>(value)));
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_ENDIAN_BYTE_SWAP_HPP_
