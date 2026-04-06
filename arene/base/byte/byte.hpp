// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BYTE_BYTE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BYTE_BYTE_HPP_

// IWYU pragma: private, include "arene/base/byte.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/attributes.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/utility/to_underlying.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

/// @brief A type that represents a byte of data.
///
/// Use this for cases where you are dealing with serialised data in bytes
/// or inspecting byte-representations of values.
///
/// This type is equivalent to std::byte added in C++17.
///
/// This type supports only bit-wise operations and does not support arithmetic
/// operations like +, -, *, / or %.
// Need to tag the type as "may_alias" to have the same aliasing behaviour
// as 'char', 'unsigned char' and 'std::byte'.
enum class ARENE_MAY_ALIAS byte : std::uint8_t {};

/// @brief Convert an integer to an @c arene::base::byte
/// @tparam ValueType The type of the integer.
/// @param val The value to convert
/// @return The supplied value, cast to @c byte
template <typename ValueType, constraints<std::enable_if_t<std::is_integral<ValueType>::value>> = nullptr>
constexpr auto to_byte(ValueType const val) noexcept -> byte {
  // parasoft-begin-suppress AUTOSAR-A7_2_1-a-2 "All uint8 values are valid values for byte"
  // parasoft-begin-suppress CERT_CPP-INT50-a-3 "All uint8 values are valid values for byte"
  return static_cast<byte>(static_cast<std::uint8_t>(val));
  // parasoft-end-suppress AUTOSAR-A7_2_1-a-2
  // parasoft-end-suppress CERT_CPP-INT50-a-3
}

/// @brief convert the byte value to an integral representation.
/// @tparam IntegerType The type of integer to convert to.
/// @param val The byte to convert.
/// @return The integral value of @c val.
template <typename IntegerType, constraints<std::enable_if_t<std::is_integral<IntegerType>::value>> = nullptr>
constexpr auto to_integer(byte const val) noexcept -> IntegerType {
  return static_cast<IntegerType>(to_underlying(val));
}

// parasoft-begin-suppress AUTOSAR-M5_8_1-a "False positive: There is a precondition check before the shift"

/// @brief Left-shift the bits of @c val by @c shift.
/// @tparam IntegerType The type of integer to shift by
/// @param val The byte to shift
/// @param shift The number of bits to shift by
/// @return The result of shifting @c val left by @c shift bits
/// @pre <c>shift < CHAR_BIT</c>, otherwise ARENE_PRECONDITION violation
template <typename IntegerType, constraints<std::enable_if_t<std::is_integral<IntegerType>::value>> = nullptr>
constexpr auto operator<<(byte const val, IntegerType const shift) noexcept -> byte {
  static_assert(!std::is_signed<IntegerType>::value, "Shifting by a signed integer is not permitted");

  ARENE_PRECONDITION(shift < static_cast<IntegerType>(CHAR_BIT));

  return to_byte(static_cast<std::uint32_t>(to_underlying(val)) << shift);
}

/// @brief Right-shift the bits of @c val by @c shift.
/// @tparam IntegerType The type of integer to shift by.
/// @param val The byte to shift
/// @param shift The number of bits to shift by
/// @return The result of shifting @c val right by @c shift bits
/// @pre <c>shift < CHAR_BIT</c>, otherwise ARENE_PRECONDITION violation
template <typename IntegerType, constraints<std::enable_if_t<std::is_integral<IntegerType>::value>> = nullptr>
constexpr auto operator>>(byte const val, IntegerType const shift) noexcept -> byte {
  static_assert(!std::is_signed<IntegerType>::value, "Shifting by a signed integer is not permitted");

  ARENE_PRECONDITION(shift < static_cast<IntegerType>(CHAR_BIT));

  return to_byte(static_cast<std::uint32_t>(to_underlying(val)) >> shift);
}

// parasoft-end-suppress AUTOSAR-M5_8_1-a

// parasoft-begin-suppress AUTOSAR-M5_17_1-a-2 "False positive: The compound operator uses the non-compound operator in
// the implementation"
/// @brief Bit-wise or of the byte representations.
/// @param lhs The first value
/// @param rhs The second value
/// @return The result of bitwise or of @c lhs and @c rhs
constexpr auto operator|(byte const lhs, byte const rhs) noexcept -> byte {
  return to_byte(static_cast<std::uint8_t>(to_underlying(lhs) | to_underlying(rhs)));
}

/// @brief Bit-wise and of the byte representations.
/// @param lhs The first value
/// @param rhs The second value
/// @return The result of bitwise and of @c lhs and @c rhs
constexpr auto operator&(byte const lhs, byte const rhs) noexcept -> byte {  // CODEQLFP(M5-3-3)
  return to_byte(static_cast<std::uint8_t>(to_underlying(lhs) & to_underlying(rhs)));
}

/// @brief Bit-wise xor of the byte representations.
/// @param lhs The first value
/// @param rhs The second value
/// @return The result of bitwise xor of @c lhs and @c rhs
constexpr auto operator^(byte const lhs, byte const rhs) noexcept -> byte {
  return to_byte(static_cast<std::uint8_t>(to_underlying(lhs) ^ to_underlying(rhs)));
}
// parasoft-end-suppress AUTOSAR-M5_17_1-a-2

/// @brief Bit-wise negation of the byte representation.
/// @param val The value to negate
/// @return The result of bitwise negation of @c val
constexpr auto operator~(byte const val) noexcept -> byte {
  return to_byte(~static_cast<std::uint32_t>(to_underlying(val)));
}

/// @brief Modify @c val by shifting its bits left by @c shift.
/// @param val The value to shift
/// @param shift The number of bits to shift by
/// @return A reference to @c val
template <typename IntegerType, constraints<std::enable_if_t<std::is_integral<IntegerType>::value>> = nullptr>
constexpr auto operator<<=(byte& val, IntegerType const shift) noexcept -> byte& {
  val = val << shift;
  return val;
}

/// @brief Modify @c val by shifting its bits right by @c shift.
/// @param val The value to shift
/// @param shift The number of bits to shift by
/// @return A reference to @c val
template <typename IntegerType, constraints<std::enable_if_t<std::is_integral<IntegerType>::value>> = nullptr>
constexpr auto operator>>=(byte& val, IntegerType const shift) noexcept -> byte& {
  val = val >> shift;
  return val;
}

/// @brief Modify @c lhs by bit-wise or'ing it with the bits of @c rhs.
/// @param lhs The value to modify
/// @param rhs The second value
/// @return A reference to @c lhs
constexpr auto operator|=(byte& lhs, byte const rhs) noexcept -> byte& {
  lhs = lhs | rhs;
  return lhs;
}

/// @brief Modify @c lhs by bit-wise and'ing it with the bits of @c rhs.
/// @param lhs The value to modify
/// @param rhs The second value
/// @return A reference to @c lhs
constexpr auto operator&=(byte& lhs, byte const rhs) noexcept -> byte& {
  lhs = lhs & rhs;
  return lhs;
}

/// @brief Modify @c lhs by bit-wise xor'ing it with the bits of @c rhs.
/// @param lhs The value to modify
/// @param rhs The second value
/// @return A reference to @c val
constexpr auto operator^=(byte& lhs, byte const rhs) noexcept -> byte& {
  lhs = lhs ^ rhs;
  return lhs;
}

namespace literals {

///
/// @brief Literal helper for creating @c byte instances.
///
/// @param value The literal to cast to @c byte .
/// @return byte Equivalent to calling @c to_byte(value) .
///
// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "False positive: UDLs must take long long"
// NOLINTNEXTLINE (google-runtime-init) literal operator
constexpr byte operator""_byte(unsigned long long int const value) noexcept { return to_byte(value); }
// parasoft-end-suppress AUTOSAR-A3_9_1-b-2

}  // namespace literals

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_BYTE_BYTE_HPP_
