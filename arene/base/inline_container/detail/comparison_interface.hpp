// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_COMPARISON_INTERFACE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_COMPARISON_INTERFACE_HPP_

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/algorithm/equal.hpp"
#include "arene/base/algorithm/lexicographical_compare.hpp"
#include "arene/base/compare/compare_three_way.hpp"
#include "arene/base/compare/operators.hpp"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// IWYU pragma: private
// IWYU pragma: friend "arene/base/inline_container/.*"

namespace arene {
namespace base {
namespace inline_container {
namespace detail {

/// @brief helper class to define comparison operations for containers
/// @tparam Derived derived type providing container basis functions
///
/// CRTP helper class to try to define the three-way comparison functions used by the @c
/// generic_ordering_from_three_way_compare class. If the value_type of the container supports three_way_compare, then
/// the three_way_compare function will be generated as a lexicographical comparison. Additionally, the
/// fast_inequality_check will be generated based on the current container size.
///
/// If the underlying type does not support three_way_compare, then equality and inequality operators will be provided.
///
/// The derived type is required to provide the following typedefs:
/// * @c value_type
///
/// The derived type is required to implement the following basis functions:
/// * <tt> auto begin() </tt>
/// * <tt> auto end() </tt>
/// * <tt> auto size() </tt>
template <class Derived>
class comparison_interface : public generic_ordering_from_three_way_compare_and_equals<Derived> {
  /// @brief The ordering base type
  using ordering_base = generic_ordering_from_three_way_compare_and_equals<Derived>;

 protected:
  /// @brief Construct a 'comparison_interface' CRTP helper type.
  ///
  /// Intended to be invoked as:
  /// ~~~{.cpp}
  /// comparison_interface{this}
  /// ~~~
  ///
  /// This constructor exists to prevent use by a type that does _not_ define
  /// 'comparison_interface' as a CRTP base class or to detect mismatch between the actual derived type and the @c
  /// Derived template parameter.
  ///
  template <class Actual, class Expected = Derived>
  explicit constexpr comparison_interface(Actual*) noexcept
      : ordering_base{} {
    static_assert(
        std::is_same<Actual, Expected>::value,
        "'Derived' type mismatch.\n"  //
        "Please verify that 'Derived' is defined correctly when using 'comparison_interface' as a CRTP base."
    );
  }

 public:
  // parasoft-begin-suppress AUTOSAR-A11_3_1-a-2 "False positive: It is allowed to declare comparison operators as
  // parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
  // friend functions"
  /// @brief Compare two @c Deriveds for equality.
  ///
  /// @tparam D dummy parameter which should be equal to Derived (used to enable sfinae)
  /// @tparam U dummy parameter to disable if @c value_type is three-way-comparable
  /// @param lhs The first @c Derived
  /// @param rhs The second @c Derived
  /// @return bool equivalent to @c arene::base::equal(lhs.begin(),lhs.end(),rhs.begin(),rhs.end()) .
  template <
      typename D = Derived,
      typename U = typename D::value_type,
      constraints<std::enable_if_t<is_equality_comparable_v<U>>> = nullptr>
  friend constexpr auto operator==(Derived const& lhs, Derived const& rhs) noexcept -> bool {
    return ::arene::base::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  /// @brief Compare two @c Deriveds for inequality.
  ///
  /// @tparam D dummy parameter which should be equal to Derived (used to enable sfinae)
  /// @tparam U dummy parameter to disable if @c value_type is three-way-comparable
  /// @param lhs The first @c Derived
  /// @param rhs The second @c Derived
  /// @return bool equivalent to @c !(lhs==rhs) .
  template <
      typename D = Derived,
      typename U = typename D::value_type,
      constraints<std::enable_if_t<is_equality_comparable_v<U>>> = nullptr>
  friend constexpr auto operator!=(Derived const& lhs, Derived const& rhs) noexcept -> bool {
    return !(lhs == rhs);
  }
  // parasoft-end-suppress AUTOSAR-M3_3_2-a-2
  // parasoft-end-suppress AUTOSAR-A11_3_1-a-2

  /// @brief Three-way-compare two @c Deriveds for lexicographical ordering.
  ///
  /// @tparam D dummy parameter which should be equal to Derived (used to enable sfinae)
  /// @tparam U dummy parameter to disable if @c value_type is not three-way-comparable
  /// @param lhs The first @c Derived
  /// @param rhs The second @c Derived
  /// @return strong_ordering equivalent to
  ///          @c arene::base::lexicographical_compare_three_way(lhs.begin(),lhs.end(),rhs.begin(),rhs.end()) .
  template <
      typename D = Derived,
      typename U = typename D::value_type,
      constraints<std::enable_if_t<compare_three_way_supported_v<U>>> = nullptr>
  static constexpr auto three_way_compare(Derived const& lhs, Derived const& rhs) noexcept -> strong_ordering {
    return ::arene::base::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  /// @brief quick check that @c Deriveds have the same size to quickly identify if they are non-equal.
  ///
  /// @param first The first @c Derived to compare.
  /// @param second The second @c Derived to compare.
  /// @return @c inequality_heuristic::definitely_not_equal if the @c Deriveds have different sizes,
  /// @c inequality_heuristic::may_be_equal_or_not_equal otherwise.
  static constexpr auto fast_inequality_check(Derived const& first, Derived const& second) noexcept
      -> inequality_heuristic {
    return (first.size() == second.size()) ? inequality_heuristic::may_be_equal_or_not_equal
                                           : inequality_heuristic::definitely_not_equal;
  }
};

}  // namespace detail
}  // namespace inline_container
}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_INLINE_CONTAINER_DETAIL_COMPARISON_INTERFACE_HPP_
