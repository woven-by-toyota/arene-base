// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::to_inline_string"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_FORMAT_TO_INLINE_STRING_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_FORMAT_TO_INLINE_STRING_HPP_

// IWYU pragma: private, include "arene/base/inline_string.hpp"
// IWYU pragma: friend "arene/base/strings/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/array/array.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/detail/raw_c_string.hpp"
#include "arene/base/math/power_of_2.hpp"
#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/ignore.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_signed.hpp"
#include "arene/base/stdlib_choice/is_unsigned.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/strings/inline_string.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace arene {
namespace base {

namespace to_string_detail {

/// @brief Determine the number of digits required to represent all values of a given integral type
/// @tparam T The type to get the digit count for
/// @return The minimum number of characters needed to represent a given type.
template <typename T>
inline constexpr auto digits_for_integral_value() -> std::size_t {
  constexpr std::size_t twenty_five{25U};
  constexpr std::size_t two_to_the_25th{::arene::base::power_of_2(twenty_five)};
  /// log2(10) scaled by 2^25, to allow integer calculations for required decimal digits
  constexpr std::size_t log2_of_10_scaled_by_2_to_25{111465410U};
  constexpr std::size_t signed_digit_size{std::is_signed<T>::value ? 1U : 0U};
  return ((sizeof(T) * static_cast<std::size_t>(CHAR_BIT) - signed_digit_size) * two_to_the_25th +
          log2_of_10_scaled_by_2_to_25 - 1U) /
             log2_of_10_scaled_by_2_to_25 +
         signed_digit_size;
}

/// @brief Trait that represents the number of decimal digits required to represent all values of type @c T as a string.
/// @tparam T The type to get the digit count for
template <typename T, typename = constraints<>>
class required_decimal_digits;

/// @brief Trait that represents the number of decimal digits required to represent all values of an integral type @c T
/// as a string.
/// @tparam T The type to get the digit count for
template <typename T>
class required_decimal_digits<T, constraints<std::enable_if_t<std::is_integral<T>::value>>>
    : public std::integral_constant<std::size_t, digits_for_integral_value<T>()> {};

/// @brief Trait that represents the number of decimal digits required to represent all values of @c bool as a string
template <>
// NOLINTNEXTLINE(readability-magic-numbers) This is making it not a magic number, clang-tidy just can't see it.
class required_decimal_digits<bool> : public std::integral_constant<std::size_t, 5> {};

/// @brief The number of decimal digits required to represent all values of an integral type @c T as a string.
/// @tparam T The type to get the digit count for
template <typename T>
extern constexpr auto required_decimal_digits_v = required_decimal_digits<T>::value;

/// @brief Check if a given number of characters is enough to hold the string representation of any value of the given
/// type
/// @return true if @c S is a sufficient number of characters to represent all values of @c T
/// @return false otherwise
template <typename T, std::size_t S>
extern constexpr bool is_large_enough_for{S >= required_decimal_digits_v<T>};

/// @brief Check if a given value is negative: this is always @c false for unsigned values
/// @tparam T The type of value to check
/// @return Always returns @c false
template <typename T, constraints<std::enable_if_t<std::is_unsigned<T>::value>> = nullptr>
constexpr auto is_negative_value(T) noexcept -> bool {
  return false;
}

/// @brief Check if a given value is negative
/// @tparam T The type of value to check
/// @param value The value to check
/// @return If the value is negative, returns @c true, else returns @c false
template <typename T, constraints<std::enable_if_t<std::is_signed<T>::value>> = nullptr>
constexpr auto is_negative_value(T value) noexcept -> bool {
  return value < 0;
}

// parasoft-begin-suppress AUTOSAR-A15_4_5-a "False positive: Does not throw"
/// @brief Obtain the character corresponding to a digit
/// @param value The digit value
/// @return The corresponding character
inline constexpr auto digit(std::uintmax_t const value) noexcept -> arene::base::detail::character {
  /// @brief A list of characters corresponding to the digits 0-9
  constexpr array<arene::base::detail::character, 10> digits{{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}};

  ARENE_PRECONDITION(value < std::uintmax_t{digits.size()});
  return digits[static_cast<std::size_t>(value)];
}
// parasoft-end-suppress AUTOSAR-A15_4_5-a

/// @brief A zero value of the specified type
/// @tparam T The type to get zero for
template <typename T>
extern constexpr T zero_v{0};

}  // namespace to_string_detail

/// @brief Produces a string representation of an input integral value in base-10.
///
/// @tparam T The type of the value to convert.
/// @tparam S The size of the inline string to write the result to. Must be large enough to hold T.
/// @param value The value to convert.
/// @return inline_string with a character representation of the input value in base-10, with no padding.
template <
    typename T,
    std::size_t S = to_string_detail::required_decimal_digits_v<T>,
    constraints<
        std::enable_if_t<std::is_integral<T>::value>,
        std::enable_if_t<to_string_detail::is_large_enough_for<T, S>>> = nullptr>
inline constexpr auto to_inline_string(T value) noexcept -> inline_string<S> {
  if (value == to_string_detail::zero_v<T>) {
    return inline_string<S>{"0"};
  }
  bool const is_negative{to_string_detail::is_negative_value(value)};
  using unsigned_t = std::make_unsigned_t<T>;
  unsigned_t unsigned_value{0U};
  if (is_negative && (value == std::numeric_limits<T>::min())) {
    unsigned_value = static_cast<unsigned_t>(power_of_2((sizeof(T) * static_cast<std::size_t>(CHAR_BIT)) - 1U));
  } else if (is_negative) {
    unsigned_value = static_cast<unsigned_t>(-value);
  } else {
    unsigned_value = static_cast<unsigned_t>(value);
  }
  constexpr unsigned_t base_divisor{10U};
  constexpr std::size_t num_digits{to_string_detail::required_decimal_digits_v<T>};

  inline_string<S> result;
  result.resize(num_digits);
  std::size_t pos{num_digits - 1U};
  while (unsigned_value != 0U) {
    result[pos] = to_string_detail::digit(unsigned_value % base_divisor);
    unsigned_value = unsigned_value / base_divisor;
    --pos;
  }
  // remove unused digits since we filled from right to left, and insert '-' if needed.
  if (is_negative) {
    std::ignore = result.erase(0U, pos);
    result[0U] = '-';
  } else {
    std::ignore = result.erase(0U, pos + 1U);
  }
  return result;
}

/// @brief Produces a string representation of an input boolean value.
///
/// @tparam S The size of the inline string to write the result to. Must be at least 5 characters.
/// @param value The value to convert.
/// @return inline_string with the boolean represented as either "true" or "false".
template <
    typename T = bool,
    std::size_t S = to_string_detail::required_decimal_digits_v<T>,
    constraints<std::enable_if_t<to_string_detail::is_large_enough_for<T, S>>> = nullptr>
inline constexpr auto to_inline_string(bool value) noexcept -> inline_string<S> {
  return inline_string<S>{value ? "true" : "false"};
}

}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_STRINGS_FORMAT_TO_INLINE_STRING_HPP_
