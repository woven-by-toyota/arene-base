// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_NAN_REJECTING_DISTRIBUTION_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_NAN_REJECTING_DISTRIBUTION_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_arithmetic.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_floating_point.hpp"

namespace arene {
namespace base {
namespace testing {

/// @brief A distribution adaptor that discards NaN values drawn from the wrapped distribution and redraws
/// @tparam Distribution A distribution return floating point values
template <class Distribution>
class nan_rejecting_distribution {
 public:
  /// @brief The value type produced by this adaptor
  using result_type = typename Distribution::result_type;

  static_assert(
      std::is_arithmetic<result_type>::value,
      "nan_rejecting_distribution only makes sense for arithmetic distributions"
  );

  /// @brief Default construct adapted distribution
  template <  //
      class T = Distribution,
      constraints<std::enable_if_t<std::is_default_constructible<T>::value>> = nullptr>
  constexpr nan_rejecting_distribution() noexcept
      : dist_{} {}

  /// @brief Construct the adaptor wrapping the given distribution
  /// @param dist The distribution to adapt
  explicit constexpr nan_rejecting_distribution(Distribution dist) noexcept
      : dist_{dist} {}

  /// @brief Draw a non-NaN value from the wrapped distribution; SFINAE overload for floating-point types
  /// @tparam PRNG A bit generator (PRNG) type satisfying UniformRandomBitGenerator
  /// @param prng A bit generator (PRNG) instance
  /// @return A value drawn from the wrapped distribution with NaN results discarded and redrawn
  template <
      class PRNG,
      class T = result_type,
      constraints<std::enable_if_t<std::is_floating_point<T>::value>> = nullptr>
  constexpr auto operator()(PRNG& prng) const noexcept(noexcept(std::declval<Distribution const&>()(prng)))
      -> result_type {
    auto value = dist_(prng);
    while (isnan(value)) {
      value = dist_(prng);
    }
    return value;
  }

  /// @brief Draw a non-NaN value from the wrapped distribution; SFINAE overload for non-floating-point types
  /// @tparam PRNG A bit generator (PRNG) type satisfying UniformRandomBitGenerator
  /// @param prng A bit generator (PRNG) instance
  /// @return Any value drawn from the wrapped distribution; it's assumed that NaNs can not actually be generated
  template <
      class PRNG,
      class T = result_type,
      constraints<std::enable_if_t<!std::is_floating_point<T>::value>> = nullptr>
  constexpr auto operator()(PRNG& prng) const noexcept(noexcept(std::declval<Distribution const&>()(prng)))
      -> result_type {
    return dist_(prng);
  }

 private:
  Distribution dist_;
};

}  // namespace testing
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TESTING_NAN_REJECTING_DISTRIBUTION_HPP_
