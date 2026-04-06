// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPARE_COMPARE_THREE_WAY_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPARE_COMPARE_THREE_WAY_HPP_

// IWYU pragma: private, include "arene/base/compare.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

// parasoft-begin-suppress AUTOSAR-A16_2_2-a-2 "Arene Base aggregate headers permitted by A16-2-2 Permit #1"
#include "arene/base/compare/strong_ordering.hpp"
#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/equal_to.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_pointer.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
#include "arene/base/stdlib_choice/less.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/utility/safe_comparisons.hpp"
// parasoft-end-suppress AUTOSAR-A16_2_2-a-2

// parasoft-begin-suppress AUTOSAR-A15_4_3-a-2 "False positive: This file is the only declaration of these functions"

namespace arene {
namespace base {

/// @brief Type trait to check if an instance of type @c T can be compared to an instance of type @c U with a static
/// three-way-compare member function that returns @c strong_ordering.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return true If @c T::three_way_compare(T,U) is defined and returns @c strong_ordering .
/// @return false Otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_three_way_comparable_v = false;

/// @brief Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool is_three_way_comparable_v<
    T,
    U,
    constraints<std::enable_if_t<std::is_same<
        decltype(T::three_way_compare(std::declval<T const&>(), std::declval<U const&>())),
        strong_ordering>::value>>> = true;

namespace three_way_compare_detail {

/// @brief Class for callable objects that performs three-way comparison between two values. It provides a transparent
/// comparison that depends on the comparisons provided by the underlying types.
///
/// Provides @c operator() that takes two values and returns a @c strong_ordering with the result of the comparison.
///
/// The order of preference for performing the comparison is as follows:
/// 1. If the type of @c lhs side provides a @c three_way_compare static member function which is well-formed when
///    applied to the type of @c rhs, that is used
/// 1. If both <c>lhs < rhs</c> and <c>lhs == rhs</c> are well-formed, they are used
/// 1. if only <c> lhs < rhs</c> and <c> rhs < lhs</c> are well-formed, they are used
/// If none of those overloads are valid, the comparison cannot be made.
///
class compare_three_way {
 public:
  /// @brief Member that declares this is a transparent comparator.
  using is_transparent = void;

  /// @brief Compare two pointers using @c std::less
  /// @tparam T The type of @c lhs
  /// @tparam U The type of @c rhs
  /// @param lhs The left hand operand to compare against
  /// @param rhs The right hand operand to compare against
  /// @return @c strong_ordering::equal if the values are equal, @c
  /// strong_ordering::less if the first operand is less than the second
  /// operand, @c strong_ordering::greater otherwise
  template <
      typename T,
      typename U,
      constraints<std::enable_if_t<std::is_pointer<T>::value>, std::enable_if_t<std::is_pointer<U>::value>> = nullptr>
  constexpr auto operator()(T const& lhs, U const& rhs) const noexcept -> strong_ordering {
    if (std::less<>{}(lhs, rhs)) {
      return strong_ordering::less;
    }
    if (std::less<>{}(rhs, lhs)) {
      return strong_ordering::greater;
    }
    return strong_ordering::equal;
  }

  /// @brief Compare two integral objects using less-than and equality in a sign-conversion safe way.
  /// @tparam T The type of @c lhs
  /// @tparam U The type of @c rhs
  /// @param lhs The left hand operand to compare against
  /// @param rhs The right hand operand to compare against
  /// @return @c strong_ordering::equal if the values are equal
  /// @return @c strong_ordering::less if the first operand is less than the second operand. Negative signed values
  ///             always compare less than unsigned values.
  /// @return @c strong_ordering::greater Otherwise.
  template <
      typename T,
      typename U,
      constraints<std::enable_if_t<std::is_integral<T>::value>, std::enable_if_t<std::is_integral<U>::value>> = nullptr>
  constexpr auto operator()(T const& lhs, U const& rhs) const noexcept -> strong_ordering {
    if (::arene::base::cmp_equal(lhs, rhs)) {
      return strong_ordering::equal;
    }
    if (::arene::base::cmp_less(lhs, rhs)) {
      return strong_ordering::less;
    }
    return strong_ordering::greater;
  }

  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Three-way comparison requires equality check");

  /// @brief Compare two objects using less-than and equality.
  /// @tparam T The type of @c lhs
  /// @tparam U The type of @c rhs
  /// @param lhs The left hand operand to compare against
  /// @param rhs The right hand operand to compare against
  /// @return @c strong_ordering::equal if the values are equal, @c strong_ordering::less if the first operand is less
  ///          than the second operand, @c strong_ordering::greater otherwise
  template <
      typename T,
      typename U,
      constraints<
          std::enable_if_t<!is_three_way_comparable_v<T, U>>,
          std::enable_if_t<!is_three_way_comparable_v<U, T>>,
          std::enable_if_t<!std::is_pointer<T>::value || !std::is_pointer<U>::value>,
          std::enable_if_t<!std::is_integral<T>::value || !std::is_integral<U>::value>,
          std::enable_if_t<base::is_less_than_comparable_v<T, U>>,
          std::enable_if_t<base::is_equality_comparable_v<T, U>>> = nullptr>
  constexpr auto operator()(T const& lhs, U const& rhs) const
      noexcept(base::is_nothrow_less_than_comparable_v<T, U> && base::is_nothrow_equality_comparable_v<T, U>)
          -> strong_ordering {
    if (std::equal_to<>{}(lhs, rhs)) {
      return strong_ordering::equal;
    }
    if (std::less<>{}(lhs, rhs)) {
      return strong_ordering::less;
    }
    return strong_ordering::greater;
  }

  ARENE_IGNORE_END();

  /// @brief Compare two objects using less-than
  /// @tparam T The type of @c lhs
  /// @tparam U The type of @c rhs
  /// @param lhs The left hand operand to compare against
  /// @param rhs The right hand operand to compare against
  /// @return @c strong_ordering::less if the first operand is less than the second operand, @c
  ///          strong_ordering::greater if the second is less than the first, @c strong_ordering::equal otherwise.
  template <
      typename T,
      typename U,
      constraints<
          std::enable_if_t<!is_three_way_comparable_v<T, U>>,
          std::enable_if_t<!is_three_way_comparable_v<U, T>>,
          std::enable_if_t<!std::is_pointer<T>::value || !std::is_pointer<U>::value>,
          std::enable_if_t<base::is_less_than_comparable_v<T, U>>,
          std::enable_if_t<!base::is_equality_comparable_v<T, U>>,
          std::enable_if_t<base::is_less_than_comparable_v<U, T>>> = nullptr>
  constexpr auto operator()(T const& lhs, U const& rhs) const
      noexcept(base::is_nothrow_less_than_comparable_v<T, U> && base::is_nothrow_less_than_comparable_v<U, T>)
          -> strong_ordering {
    if (std::less<>{}(lhs, rhs)) {
      return strong_ordering::less;
    }
    if (std::less<>{}(rhs, lhs)) {
      return strong_ordering::greater;
    }
    return strong_ordering::equal;
  }

  /// @brief Compare two objects using a @c three_way_compare static member function.
  /// @tparam T The type of @c lhs
  /// @tparam U The type of @c rhs
  /// @param lhs The left hand operand to compare against
  /// @param rhs The right hand operand to compare against
  /// @return The result of @c T::three_way_compare(t,u)
  template <typename T, typename U, constraints<std::enable_if_t<is_three_way_comparable_v<T, U>>> = nullptr>
  constexpr auto operator()(T const& lhs, U const& rhs) const
      noexcept(noexcept(T::three_way_compare(std::declval<T const&>(), std::declval<U const&>()))) -> strong_ordering {
    return T::three_way_compare(lhs, rhs);
  }

  /// @brief Compare two objects using a @c three_way_compare static member function.
  /// @tparam T The type of @c lhs
  /// @tparam U The type of @c rhs
  /// @param lhs The left hand operand to compare against
  /// @param rhs The right hand operand to compare against
  /// @return strong_ordering The opposite result of @c U::three_way_compare(u,t)
  template <
      typename T,
      typename U,
      constraints<
          std::enable_if_t<!is_three_way_comparable_v<T, U>>,
          std::enable_if_t<is_three_way_comparable_v<U, T>>> = nullptr>
  constexpr auto operator()(T const& lhs, U const& rhs) const
      noexcept(noexcept(U::three_way_compare(std::declval<U const&>(), std::declval<T const&>()))) -> strong_ordering {
    return opposite_ordering(U::three_way_compare(rhs, lhs));
  }
};

}  // namespace three_way_compare_detail

// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: @brief tag is present"
/// @brief Function object that does a three-way comparison between operands, equivalent to @c std::compare_three_way .
using compare_three_way = three_way_compare_detail::compare_three_way;
// parasoft-end-suppress AUTOSAR-A2_7_3-a-2

/// @brief Boolean constant indicating if instances of @c Lhs and @c Rhs can be compared using @c compare_three_way
/// @tparam Lhs The type of the left-hand operand
/// @tparam Rhs The type of the right-hand operand
/// @{
template <typename Lhs, typename Rhs = Lhs, typename = constraints<>>
extern constexpr bool compare_three_way_supported_v = false;

template <typename Lhs, typename Rhs>
extern constexpr bool compare_three_way_supported_v<
    Lhs,
    Rhs,
    constraints<decltype(compare_three_way{}(std::declval<Lhs>(), std::declval<Rhs>()))>> = true;
/// @}

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_COMPARE_COMPARE_THREE_WAY_HPP_
