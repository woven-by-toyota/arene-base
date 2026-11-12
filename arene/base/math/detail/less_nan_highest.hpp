// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines arene::base::math_detail::less_nan_highest"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_LESS_NAN_HIGHEST_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_LESS_NAN_HIGHEST_HPP_

// IWYU pragma: private
// IWYU pragma: friend "arene/base/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compiler_support/cpp14_inline.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/math/float_properties.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/type_traits/is_invocable.hpp"
#include "arene/base/type_traits/is_predicate.hpp"

namespace arene {
namespace base {
namespace math_detail {

/// @brief Implementation for the @c less_nan_highest function object
///
/// Uses <c>std::less<></c> to compare values of a type, with all NaN values
/// considered equivalent and ordered above all non-NaN values. This is intended
/// to match the C23 fmaximum's treatment of NaN arguments.
///
/// @note This does NOT match fmaximum's signed-zero behavior: fmaximum
/// orders -0 < +0, whereas this comparator delegates to <c>std::less<></c> and
/// treats -0 and +0 as equivalent.
class less_nan_highest_fn {
 public:
  /// @brief Compares two values
  /// @tparam T type to compare
  /// @param lhs left hand operand of the comparison
  /// @param rhs right hand operand of the comparison
  /// @return @c true if @p lhs orders before @p rhs, otherwise @c false
  /// @note This overload compares two values that cannot be NaN
  template <
      class T,
      constraints<
          std::enable_if_t<is_predicate_v<std::less<>&&, T const&, T const&>>,
          std::enable_if_t<!is_predicate_v<decltype(arene::base::isnan), T const&>>> = nullptr>
  constexpr auto operator()(T const& lhs, T const& rhs) const noexcept(  //
      is_nothrow_invocable_v<std::less<>&&, T const&, T const&>          //
  ) -> bool {
    return std::less<>{}(lhs, rhs);
  }

  /// @brief Compares two values
  /// @tparam T type to compare
  /// @param lhs left hand operand of the comparison
  /// @param rhs right hand operand of the comparison
  /// @return @c true if @p lhs orders before @p rhs, otherwise @c false
  /// @note This overload compares two values that may be NaN. All NaN values
  /// are treated as equivalent to each other and as ordered above every non-NaN
  /// value, so that the result is a strict weak ordering (unlike
  /// <c>std::less<></c>, which is not a strict weak ordering when NaNs are
  /// present)
  /// @pre For any NaN value of @c T, @c operator< must yield @c false (i.e. NaN
  /// compares as unordered, per IEEE 754).
  template <
      class T,
      constraints<
          std::enable_if_t<is_predicate_v<std::less<>&&, T const&, T const&>>,
          std::enable_if_t<is_predicate_v<decltype(arene::base::isnan), T const&>>> = nullptr>
  constexpr auto operator()(T const& lhs, T const& rhs) const noexcept(  //
      is_nothrow_invocable_v<std::less<>&&, T const&, T const&> &&       //
      is_nothrow_invocable_v<decltype(arene::base::isnan), T const&>     //
  ) -> bool {
    using arene::base::isnan;
    if (isnan(lhs) || isnan(rhs)) {
      return (!isnan(lhs)) && isnan(rhs);
    }
    return std::less<>{}(lhs, rhs);
  }
};

/// @copydoc arene::base::math_detail::less_nan_highest_fn::operator()
// parasoft-begin-suppress AUTOSAR-M7_3_3-a "Unnamed namespace creates a per-TU reference to a single global object."
// parasoft-begin-suppress CERT_CPP-DCL59-a "Unnamed namespace creates a per-TU reference to a single global object."
ARENE_CPP14_INLINE_VARIABLE(math_detail::less_nan_highest_fn, less_nan_highest);
// parasoft-end-suppress AUTOSAR-M7_3_3-a
// parasoft-end-suppress CERT_CPP-DCL59-a

}  // namespace math_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MATH_DETAIL_LESS_NAN_HIGHEST_HPP_
