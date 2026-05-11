// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COPY_N_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COPY_N_HPP_

// IWYU pragma: private, include <algorithm>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/conditional.hpp"
#include "stdlib/include/stdlib_detail/cstdint.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/enable_if.hpp"
#include "stdlib/include/stdlib_detail/is_convertible.hpp"
#include "stdlib/include/stdlib_detail/is_signed.hpp"
#include "stdlib/include/stdlib_detail/iterator_concepts.hpp"
#include "stdlib/include/stdlib_detail/iterator_traits.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax permitted by A7-1-5 Permit #1 v1.0.0"

namespace std {

namespace copy_n_detail {
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
inline constexpr auto convert_size_to_integer(bool const value) noexcept -> uint8_t {
  if (value) {
    return 1U;
  }
  return 0U;
}

// parasoft-begin-suppress AUTOSAR-A3_9_1-b "Overloaded for every built-in integer type"
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
inline constexpr auto convert_size_to_integer(char const value) noexcept
    -> conditional_t<is_signed_v<char>, int8_t, uint8_t> {
  return static_cast<conditional_t<is_signed_v<char>, int8_t, uint8_t>>(value);
}
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
inline constexpr auto convert_size_to_integer(unsigned char const value) noexcept -> uint8_t { return value; }
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
inline constexpr auto convert_size_to_integer(signed char const value) noexcept -> int8_t { return value; }
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
inline constexpr auto convert_size_to_integer(int const value) noexcept -> int { return value; }
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
inline constexpr auto convert_size_to_integer(unsigned const value) noexcept -> unsigned { return value; }
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
// NOLINTNEXTLINE(google-runtime-int)
inline constexpr auto convert_size_to_integer(short const value) noexcept -> short { return value; }
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
// NOLINTNEXTLINE(google-runtime-int)
inline constexpr auto convert_size_to_integer(unsigned short const value) noexcept -> unsigned short { return value; }
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
// NOLINTNEXTLINE(google-runtime-int)
inline constexpr auto convert_size_to_integer(long const value) noexcept -> long { return value; }
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
// NOLINTNEXTLINE(google-runtime-int)
inline constexpr auto convert_size_to_integer(unsigned long const value) noexcept -> unsigned long { return value; }
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
// NOLINTNEXTLINE(google-runtime-int)
inline constexpr auto convert_size_to_integer(long long const value) noexcept -> long long { return value; }
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns an integer with the same value.
/// @param value The value to convert
/// @return The value as an integer
// NOLINTNEXTLINE(google-runtime-int)
inline constexpr auto convert_size_to_integer(unsigned long long const value) noexcept -> unsigned long long {
  return value;
}
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns the input value cast to an integer.
/// @param value The value to convert
/// @return The value as an integer
inline constexpr auto convert_size_to_integer(float const value) noexcept -> std::int64_t {
  return static_cast<std::int64_t>(value);
}
/// @brief Overloaded internal function to handle Size types that are "convertible to an integer" rather than being an
/// integer themselves. Each overload acceps a specific type, and returns the input value cast to an integer.
/// @param value The value to convert
/// @return The value as an integer
inline constexpr auto convert_size_to_integer(double const value) noexcept -> std::int64_t {
  return static_cast<std::int64_t>(value);
}
// parasoft-end-suppress AUTOSAR-A3_9_1-b

/// @brief Type trait to check if a type is implicitly convertible to an integral type. The value is @c true if so, @c
/// false otherwise
/// @tparam T The type to check
template <typename T, typename = arene::base::constraints<>>
extern constexpr bool is_convertible_to_integral_v = false;

/// @brief Type trait to check if a type is implicitly convertible to an integral type. The value is @c true if so, @c
/// false otherwise
/// @tparam T The type to check
template <typename T>
extern constexpr bool is_convertible_to_integral_v<
    T,
    arene::base::constraints<decltype(std::copy_n_detail::convert_size_to_integer(declval<T>()))>> = true;

/// @brief Type trait to check if a type is implicitly convertible to an integral type without throwing. The value is @c
/// true if so, @c false otherwise
/// @tparam T The type to check
template <typename T, typename = arene::base::constraints<>>
extern constexpr bool is_nothrow_convertible_to_integral_v = false;

/// @brief Type trait to check if a type is implicitly convertible to an integral type without throwing. The value is @c
/// true if so, @c false otherwise
/// @tparam T The type to check
template <typename T>
extern constexpr bool
    is_nothrow_convertible_to_integral_v<T, arene::base::constraints<enable_if_t<is_convertible_to_integral_v<T>>>> =
        noexcept(std::copy_n_detail::convert_size_to_integer(declval<T>()));

}  // namespace copy_n_detail

// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief Copy @c count elements from the input range starting with @c first to the output range starting with @c out
/// @tparam InputIterator The type of the input iterator
/// @tparam Size The type of the count of elements to copy
/// @tparam OutputIterator The type of the output iterator
/// @param first The start of the source range
/// @param count The number of elements to copy
/// @param out The start of the output range
/// @return The final value of @c out
/// @pre @c InputIterator must be an input iterator
/// @pre @c first must be the start of a valid input range of at least @c count elements
/// @pre @c Size must be convertible to an integral type
/// @pre @c OutputIterator must be an output iterator
/// @pre @c out must be the start of a valid output range of at least @c count elements
template <
    typename InputIterator,
    typename Size,
    typename OutputIterator,
    arene::base::constraints<
        enable_if_t<internal::has_basic_input_iterator_operations_v<InputIterator>>,
        enable_if_t<copy_n_detail::is_convertible_to_integral_v<Size>>,
        enable_if_t<internal::has_basic_output_iterator_operations_v<OutputIterator>>,
        enable_if_t<internal::is_indirectly_copy_assignable_v<InputIterator, OutputIterator>>> = nullptr>
auto copy_n(
    InputIterator first,
    Size count,
    OutputIterator out
) noexcept(internal::has_nothrow_basic_input_iterator_operations_v<InputIterator> && internal::has_nothrow_basic_output_iterator_operations_v<OutputIterator> && copy_n_detail::is_nothrow_convertible_to_integral_v<Size> && internal::is_indirectly_nothrow_copy_assignable_v<InputIterator, OutputIterator>)
    -> OutputIterator {
  // parasoft-begin-suppress AUTOSAR-M5_0_4-a "False positive: No signed/unsigned conversion here"
  auto count_to_copy = copy_n_detail::convert_size_to_integer(count);
  // parasoft-end-suppress AUTOSAR-M5_0_4-a
  using count_type = decltype(count_to_copy);

  for (; count_to_copy > static_cast<count_type>(0); --count_to_copy) {
    *out = *first;
    // parasoft-begin-suppress AUTOSAR-M5_0_15-a-2 "This is an iterator type, so incrementing is OK"
    ++out;
    ++first;
    // parasoft-end-suppress AUTOSAR-M5_0_15-a-2
  }
  return out;
}
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_COPY_N_HPP_
