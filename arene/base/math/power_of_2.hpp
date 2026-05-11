// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "False positive: also defines arene::base::power_of_2"

// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_POWER_OF_2_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_POWER_OF_2_HPP_

// IWYU pragma: private, include "arene/base/math.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A7_1_5-a-2 "Trailing return syntax pmeritted by A7-1-5 Permit #1 v1.0.0"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/contracts/contract.hpp"
#include "arene/base/stdlib_choice/climits.hpp"
#include "arene/base/stdlib_choice/cstddef.hpp"
#include "arene/base/stdlib_choice/cstdint.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integer_sequence.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/make_unsigned.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

namespace arene {
namespace base {

/// @brief Check if an integer is a positive power-of-two.
/// @tparam Integer An integral type.
/// @param value The value to test for a power-of-two.
/// @return @c true if the value is a positive power of two, @c false otherwise.
template <typename Integer, constraints<std::enable_if_t<std::is_integral<Integer>::value>> = nullptr>
constexpr auto is_power_of_2(Integer value) noexcept -> bool {
  /// @brief The unsigned type corresponding to the specific integer type
  using unsigned_integer = std::make_unsigned_t<Integer>;

  constexpr Integer zero{static_cast<Integer>(0)};
  constexpr unsigned_integer unsigned_zero{0U};

  // Note we need to explicitly check for zero here as otherwise zero would
  // be reported as a power-of-2 if just using the bitand operation alone.
  return (value > zero) && (static_cast<unsigned_integer>(
                                static_cast<unsigned_integer>(value) & (static_cast<unsigned_integer>(value) - 1U)
                            ) == unsigned_zero);
}

/// @brief Raises 2 to the power of a value
/// @param value The value to raise two to the power of.
/// @return std::uint64_t @c 2^value
/// @pre value < the number of bits in @c std::uint64_t , or else precondition violation.
/// @note std::size_t is 32 bits on some platforms that define std::uint64_t
inline constexpr auto power_of_2(std::uint64_t const value) noexcept -> std::uint64_t {
  ARENE_PRECONDITION(value < static_cast<std::uint64_t>(sizeof(std::uint64_t) * static_cast<std::uint64_t>(CHAR_BIT)));
  constexpr std::uint64_t one{1U};
  // parasoft-begin-suppress AUTOSAR-M5_8_1-a-2 "False positive: precondition above guarantees value is in range"
  return one << value;
  // parasoft-end-suppress AUTOSAR-M5_8_1-a-2
}

namespace power_of_2_detail {

/// @brief Return the sequence of 2^N for each N in Powers
/// @return An @c std::index_sequence holding the values
template <std::size_t... Powers>
constexpr auto make_power_of_2_sequence(std::index_sequence<>, std::index_sequence<Powers...>) noexcept {
  return std::index_sequence<power_of_2(Powers)...>();
}

/// @brief Return the sequence of 2^N for each N in Powers that is not in IgnoredPowers
/// @return An @c std::index_sequence holding the values
template <std::size_t First, std::size_t... IgnoredPowers, std::size_t... Powers>
constexpr auto
make_power_of_2_sequence(std::index_sequence<First, IgnoredPowers...>, std::index_sequence<First, Powers...>) noexcept {
  return make_power_of_2_sequence(std::index_sequence<IgnoredPowers...>{}, std::index_sequence<Powers...>{});
}

}  // namespace power_of_2_detail

/// @brief Return the sequence of 2^N for each N in the half-open range [Begin,End)
/// @return An @c std::index_sequence holding the values
template <std::size_t Begin, std::size_t End, constraints<std::enable_if_t<(End >= Begin)>> = nullptr>
constexpr auto make_power_of_2_sequence() noexcept {
  return power_of_2_detail::make_power_of_2_sequence(
      std::make_index_sequence<Begin>(),
      std::make_index_sequence<End>()
  );
}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_POWER_OF_2_HPP_
