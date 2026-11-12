// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_TESTS_FLOAT_UTILITIES_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_TESTS_FLOAT_UTILITIES_HPP_

#include "arene/base/bit/bit_cast.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/math/ulp_distance.hpp"  // IWYU pragma: keep
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/testing/bitwise_uniform_distribution.hpp"
#include "arene/base/type_manipulation/smallest_integer_for.hpp"
#include "testlibs/utilities/float32.hpp"

/// @cond
template <typename T>
constexpr auto arene::base::enable_ulp_distance_comparable<
    T,
    arene::base::constraints<std::enable_if_t<std::is_same<T, ::testing::float32>::value>>> = true;
/// @endcond

template <>
class arene::base::testing::bitwise_uniform_distribution<::testing::float32, arene::base::constraints<>> {
 public:
  template <typename Prng>
  constexpr auto operator()(Prng& prng) const noexcept -> ::testing::float32 {
    constexpr arene::base::testing::bitwise_uniform_distribution<float> inner_dist{};
    return {inner_dist(prng)};
  }
};

namespace arene {
namespace base {
namespace float_testing {

/// @brief A simplified generic implementation of std::nextafter for any IEEE-like floating point type
/// @note This assumes that the sign is the most significant bit and that the exponent is more significant than the
/// significand, but does not make any assumptions about the sizes of the exponent or significand.
template <typename FloatType>
constexpr auto nextafter(FloatType from, FloatType toward) noexcept -> FloatType {
  if (isnan(from)) {
    return from;
  }

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Intentionally checking for exact floating point equality in this check.");
  if (isnan(toward) || (from == toward)) {
    return toward;
  }
  ARENE_IGNORE_END();

  using bits_type = smallest_unsigned_integer_for_type<FloatType>;
  constexpr bits_type sign_mask{~bits_type{} - (~bits_type{} >> bits_type{1})};

  auto from_bits = bit_cast<bits_type>(from);
  auto const toward_bits = bit_cast<bits_type>(toward);

  bool const same_sign{(from_bits & sign_mask) == (toward_bits & sign_mask)};
  bool const shift_away_from_zero{same_sign && (toward_bits > from_bits)};

  if (shift_away_from_zero) {
    // Shift away from zero can not wrap for non-NaN arguments
    ++from_bits;
  } else {
    // If the starting point is a zero, flip its sign; otherwise, shrink its magnitude
    if ((from_bits & ~sign_mask) == bits_type{}) {
      from_bits ^= sign_mask;
    } else {
      --from_bits;
    }
  }

  return bit_cast<FloatType>(from_bits);
}

/// @brief Shift the argument @c start_value by @c step_count ULPs in either the positive or negative direction
template <typename FloatType, typename CountType>
auto shift_in_direction(FloatType start_value, CountType step_count, bool positive) noexcept -> FloatType {
  constexpr auto inf = std::numeric_limits<FloatType>::infinity();
  auto const direction = positive ? +inf : -inf;

  for (CountType i{}; i < step_count; ++i) {
    start_value = nextafter(start_value, direction);
  }

  return start_value;
}

}  // namespace float_testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_TESTS_FLOAT_UTILITIES_HPP_
