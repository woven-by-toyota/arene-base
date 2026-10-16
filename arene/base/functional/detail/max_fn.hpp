// parasoft-suppress AUTOSAR-A2_8_1-a AUTOSAR-A10_2_1-a "False positive: also defines
// arene::base::functional_detail::max_fn, False positive: redefines member of a privately inherited class"
// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_DETAIL_MAX_FN_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_DETAIL_MAX_FN_HPP_

#include "arene/base/stdlib_choice/conditional.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/is_copy_constructible.hpp"
#include "arene/base/stdlib_choice/is_reference.hpp"
#include "arene/base/type_manipulation/ebo_holder.hpp"
#include "arene/base/type_traits/is_compare.hpp"

namespace arene {
namespace base {
namespace functional_detail {

/// @brief compare tag
struct compare_tag {};

/// @brief function object that returns the larger of two values by a Compare
/// @tparam Compare compare function object type used to order the values
///
/// Adapts a @c Compare into a callable that, given two values, returns a
/// reference to the larger one. If values are equivalent, the first argument is
/// returned, matching the behavior of @c std::max.
// parasoft-begin-suppress AUTOSAR-A14_5_1-a "False positive: There is no template constructor"
template <class Compare>
class max_fn_t : ebo_holder<compare_tag, Compare> {
  /// @brief compare base type
  using compare_base_type = ebo_holder<compare_tag, Compare>;

 public:
  static_assert(std::is_nothrow_copy_constructible<Compare>::value, "'Compare' must be nothrow copy constructible");

  /// @brief construct a @c max_fn_t from a comparator
  /// @param cmp compare function object to adapt
  constexpr explicit max_fn_t(Compare cmp) noexcept
      : compare_base_type{static_cast<std::conditional_t<std::is_reference<Compare>::value, Compare, Compare&&> >(cmp)
        } {}

  /// @brief return the larger of two values
  /// @tparam T type of the values being compared
  /// @param lhs first value
  /// @param rhs second value
  /// @return a reference to the larger of @c lhs and @c rhs, or @c lhs on a tie
  /// @note This a detail utility and not intended to be SFINAE friendly
  template <class T>
  constexpr auto operator()(T const& lhs, T const& rhs) noexcept(
      noexcept(std::declval<Compare&>()(std::declval<T const&>(), std::declval<T const&>()))
  ) -> T const& {
    static_assert(
        is_compare_v<Compare&, T const&, T const&>,
        "'Compare' must satisfy the Compare Named Requirement with arguments 'T const&' and 'T const&'"
    );

    return (this->get_value(compare_tag{})(lhs, rhs)) ? rhs : lhs;
  }

  /// @brief return the larger of two values
  /// @tparam T type of the values being compared
  /// @param lhs first value
  /// @param rhs second value
  /// @return a reference to the larger of @c lhs and @c rhs, or @c lhs on a tie
  /// @note This a detail utility and not intended to be SFINAE friendly
  template <class T>
  constexpr auto operator()(T const& lhs, T const& rhs) const
      noexcept(noexcept(std::declval<Compare const&>()(std::declval<T const&>(), std::declval<T const&>())))
          -> T const& {
    static_assert(
        is_compare_v<Compare const&, T const&, T const&>,
        "'Compare' must satisfy the Compare Named Requirement with arguments 'T const&' and 'T const&'"
    );

    return (this->get_value(compare_tag{})(lhs, rhs)) ? rhs : lhs;
  }
};
// parasoft-end-suppress AUTOSAR-A14_5_1-a

/// @brief higher order function that converts a Compare to max function objects
/// @tparam Compare compare function object type
/// @param cmp function object to adapt
/// @return a @c max_fn_t that returns the larger of two values per @c cmp
template <class Compare>
constexpr auto max_fn(Compare const& cmp) noexcept -> max_fn_t<Compare> {
  return max_fn_t<Compare>{cmp};
}

}  // namespace functional_detail
}  // namespace base
}  // namespace arene
#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_FUNCTIONAL_DETAIL_MAX_FN_HPP_
