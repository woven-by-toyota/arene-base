// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ULP_TOLERANCE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ULP_TOLERANCE_HPP_

#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/abs_cpo.hpp"
#include "arene/base/math/ulp_distance.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/make_signed.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

// parasoft-begin-suppress AUTOSAR-M2_10_1-a "Similar names permitted by M2-10-1 Permit #1"

namespace arene {
namespace base {

namespace ulp_tolerance_detail {

// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: constructor is not templated so explicit copy not needed"
/// @brief Floating point fuzzy-equality comparator object with a particular ULP tolerance
template <typename UlpCount>
class ulp_tolerance_comparator {
  /// @brief The tolerance in ULPs (units in the last place; essentially treats the bits of a float as an integer)
  UlpCount tolerance_;

 public:
  /// @brief Construct a comparator with the given ULP tolerance
  /// @param tol The ULP tolerance to use
  constexpr explicit ulp_tolerance_comparator(UlpCount tol) noexcept
      : tolerance_(tol) {}

  /// @brief Get the ULP tolerance of this comparator
  /// @return The ULP tolerance of this comparator
  constexpr auto tolerance() const noexcept -> UlpCount { return tolerance_; }

  /// @brief Perform a floating point fuzzy-equality comparison between two numbers
  /// @tparam T A floating-point type to compare
  /// @param left First comparison argument
  /// @param right Second comparison argument
  /// @return @c true iff the absolute ULP difference between @c left and @c right is less than or equal to @c tolerance
  template <typename T, constraints<std::enable_if_t<::arene::base::enable_ulp_distance_comparable<T>>> = nullptr>
  constexpr auto operator()(T left, T right) const noexcept -> bool {
    auto const distance_result = ulp_distance<std::make_signed_t<UlpCount>>(left, right);
    if (distance_result.has_error()) {
      // This can happen if the arguments are unordered (e.g. NaNs) or their distance is too big to represent.
      // In either case, it's fair treat these as not within tolerance.
      return false;
    }

    // This abs is guaranteed not to go out of bounds because ulp_distance is never the most negative value.
    return cmp_less_equal(math::abs(*distance_result), tolerance_);
  }
};
// parasoft-end-suppress AUTOSAR-A14_5_1-a

/// @brief Function object to create fuzzy-equality comparator objects with a particular ULP tolerance
class ulp_tolerance_comparator_factory {
 public:
  /// @brief Create a floating point fuzzy-equality comparator with the given tolerance in ULPs
  /// @tparam UlpCount An integer type
  /// @param tolerance The maximum allowed ULP difference between two floating point numbers considered "equal"
  /// @c tolerance is sometimes referred to as "epsilon" but this is an ambiguous term for floating point.
  /// If <c>tolerance < 0</c> then all comparisons will return false.
  template <typename UlpCount, constraints<std::enable_if_t<std::is_integral<UlpCount>::value>> = nullptr>
  constexpr auto operator()(UlpCount tolerance) const noexcept -> ulp_tolerance_comparator<UlpCount> {
    return ulp_tolerance_comparator<UlpCount>(tolerance);
  }
};

}  // namespace ulp_tolerance_detail

// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object."
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object."
/// @brief Create a floating-point fuzzy-equality comparator with the given ULP tolerance
/// A negative tolerance makes all comparisons return false.
ARENE_CPP14_INLINE_VARIABLE(ulp_tolerance_detail::ulp_tolerance_comparator_factory, ulp_tolerance);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace base
}  // namespace arene

// parasoft-end-suppress AUTOSAR-M2_10_1-a

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_ULP_TOLERANCE_HPP_
