// parasoft-begin-suppress AUTOSAR-A2_8_1-a "False positive: also defines
// arene::base::mdspan_detail::is_representable_as"

// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_REPRESENTABLE_AS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_REPRESENTABLE_AS_HPP_

#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/numeric_limits.hpp"
#include "arene/base/utility/safe_comparisons.hpp"

namespace arene {
namespace base {
namespace mdspan_detail {

/// @brief function object implementing is_representable_as
/// @tparam Codomain integral type to check representability with
template <class Codomain>
class is_representable_as_fn  //
{
  /// @brief check if an integral value in representable in @c Codomain
  /// @tparam Integral type of the input value
  /// @param value input value
  /// @return @c true if @c value is within the bounds of @c Codomain, otherwise
  ///   @c false.
  ///
  template <class Integral>
  static constexpr auto in_range(Integral const& value) noexcept        //
      -> bool                                                           //
  {                                                                     //
    return                                                              //
        cmp_less_equal(std::numeric_limits<Codomain>::min(), value) &&  //
        cmp_less_equal(value, std::numeric_limits<Codomain>::max());
  }

  /// @brief helper to determine if runtime checks are necessary
  /// @tparam Integral type of the input value
  ///
  /// If the limits of @c Integral are bounded by the limits of @c Codomain, then
  /// any value of @c Integral is always representable as a value in @c Codomain.
  ///
  template <class Integral>
  static constexpr bool always_representable_v{
      cmp_less_equal(std::numeric_limits<Codomain>::min(), std::numeric_limits<Integral>::min()) &&
      cmp_greater_equal(std::numeric_limits<Codomain>::max(), std::numeric_limits<Integral>::max())
  };

  /// @brief check if a value is representable in @c Codomain
  /// @tparam Integral integral input value type
  /// @param value input value
  ///
  /// Overload where @c value is an integral value.
  ///
  /// @return if @c value can be represented in @c Codomain
  ///
  template <class Integral>
  static constexpr auto impl(std::true_type, Integral const& value) noexcept  //
      -> bool                                                                 //
  {
    return always_representable_v<Integral> || in_range(value);
  }

  /// @brief check if a value is representable in @c Codomain
  /// @tparam NonIntegral non-integral input value type
  ///
  /// Overload where @c value is a non-integral value.
  ///
  /// @return @c true
  ///
  template <class NonIntegral>
  static constexpr auto impl(std::false_type, NonIntegral const&) noexcept  //
      -> bool                                                               //
  {
    return true;
  }

 public:
  static_assert(  //
      std::is_integral<Codomain>::value,
      "this function is only designed for integral 'Codomain' types."
  );

  /// @brief determine if a value is representable in @c Codomain
  /// @tparam Domain type of the input value
  /// @param value input value
  ///
  /// Checks if it can be represented in type @c Codomain. If @c value :
  /// * is an integral type, checks if the value is bounded by the limits of
  ///   @c Codomain
  /// * is a non-integral type, returns @c true
  ///
  /// @note Requires:
  /// * <c> std::is_convertible<Domain, Codomain>::value </c> is @c true
  ///
  /// @return @c true if @c value is representable in @c Codomain, otherwise
  ///   @c false
  ///
  template <
      class Domain,
      constraints<std::enable_if_t<std::is_convertible<Domain const&, Codomain>::value>> = nullptr>  //
  constexpr auto operator()(Domain const& value) const noexcept                                      //
      -> bool                                                                                        //
  {
    return impl(std::is_integral<Domain>{}, value);
  }
};

/// @def arene::base::mdspan_detail::is_representable_as
/// @tparam Codomain integral type to check representability with
/// @copydoc arene::base::mdspan_detail::is_representable_as_fn::operator()
template <class Codomain>
extern constexpr auto is_representable_as = mdspan_detail::is_representable_as_fn<Codomain>{};

}  // namespace mdspan_detail
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_MDSPAN_DETAIL_IS_REPRESENTABLE_AS_HPP_
