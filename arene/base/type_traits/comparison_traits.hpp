// parasoft-begin-suppress AUTOSAR-A2_8_1-a-2 "Header defines a group of traits, so the name reflects that"
// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_COMPARISON_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_COMPARISON_TRAITS_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/declval.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"

namespace arene {
namespace base {

/// Type trait to check if an instance of type @c T can be compared to an
/// instance of type @c U with the less-than operator.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return @c true if an instance of @c T can be compared with an instance of
/// @c U, @c false otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_less_than_comparable_v = false;

/// Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool is_less_than_comparable_v<
    T,
    U,
    constraints<std::enable_if_t<std::is_convertible<decltype(std::declval<T>() < std::declval<U>()), bool>::value>>> =
    true;

/// Type trait to check if an instance of type @c T can be compared to an
/// instance of type @c U with the less-than operator without throwing an exception.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return @c true if an instance of @c T can be compared with an instance of
/// @c U without throwing, @c false otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_nothrow_less_than_comparable_v = false;

/// Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool
    is_nothrow_less_than_comparable_v<T, U, constraints<std::enable_if_t<is_less_than_comparable_v<T, U>>>> =
        noexcept(std::declval<T>() < std::declval<U>());

/// Type trait to check if an instance of type @c T can be compared to an
/// instance of type @c U with the less-than-or-equal operator.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return @c true if an instance of @c T can be compared with an instance of
/// @c U, @c false otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_less_than_or_equal_comparable_v = false;

/// Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool is_less_than_or_equal_comparable_v<
    T,
    U,
    constraints<std::enable_if_t<std::is_convertible<decltype(std::declval<T>() <= std::declval<U>()), bool>::value>>> =
    true;

/// Type trait to check if an instance of type @c T can be compared to an
/// instance of type @c U with the less-than-or-equal operator without throwing an exception.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return @c true if an instance of @c T can be compared with an instance of
/// @c U without throwing, @c false otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_nothrow_less_than_or_equal_comparable_v = false;

/// Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool is_nothrow_less_than_or_equal_comparable_v<
    T,
    U,
    constraints<std::enable_if_t<is_less_than_or_equal_comparable_v<T, U>>>> =
    noexcept(std::declval<T>() <= std::declval<U>());

/// Type trait to check if an instance of type @c T can be compared to an
/// instance of type @c U with the greater-than operator.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return @c true if an instance of @c T can be compared with an instance of
/// @c U, @c false otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_greater_than_comparable_v = false;

/// Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool is_greater_than_comparable_v<
    T,
    U,
    constraints<std::enable_if_t<std::is_convertible<decltype(std::declval<T>() > std::declval<U>()), bool>::value>>> =
    true;

/// Type trait to check if an instance of type @c T can be compared to an
/// instance of type @c U with the greater-than operator without throwing an exception.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return @c true if an instance of @c T can be compared with an instance of
/// @c U without throwing, @c false otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_nothrow_greater_than_comparable_v = false;

/// Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool
    is_nothrow_greater_than_comparable_v<T, U, constraints<std::enable_if_t<is_greater_than_comparable_v<T, U>>>> =
        noexcept(std::declval<T>() > std::declval<U>());

/// Type trait to check if an instance of type @c T can be compared to an
/// instance of type @c U with the greater-than-or-equal operator.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return @c true if an instance of @c T can be compared with an instance of
/// @c U, @c false otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_greater_than_or_equal_comparable_v = false;

/// Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool is_greater_than_or_equal_comparable_v<
    T,
    U,
    constraints<std::enable_if_t<std::is_convertible<decltype(std::declval<T>() >= std::declval<U>()), bool>::value>>> =
    true;

/// Type trait to check if an instance of type @c T can be compared to an
/// instance of type @c U with the greater-than-or-equal operator without throwing an exception.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return @c true if an instance of @c T can be compared with an instance of
/// @c U without throwing, @c false otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_nothrow_greater_than_or_equal_comparable_v = false;

/// Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool is_nothrow_greater_than_or_equal_comparable_v<
    T,
    U,
    constraints<std::enable_if_t<is_greater_than_or_equal_comparable_v<T, U>>>> =
    noexcept(std::declval<T>() >= std::declval<U>());

namespace comparison_traits_detail {
/// @brief Helper class for @c is_equality_comparable_v
class is_equality_comparable_helper {
 public:
  ARENE_IGNORE_START();
  ARENE_IGNORE_ALL("-Wfloat-equal", "Checking for existence of comparison, not using it");
  /// @brief Helper function for checking if two types are equality comparable
  /// @tparam Lhs The type of the left hand operand
  /// @tparam Rhs The type of the right hand operand
  /// @tparam Arg The deduced type of the argument
  /// @return Always returns @c true ; this overload is only selected if the types are comparable
  template <typename Lhs, typename Rhs, typename Arg, typename = decltype(std::declval<Lhs>() == std::declval<Rhs>())>
  static constexpr auto test(Arg) noexcept -> bool {
    return true;
  }
  ARENE_IGNORE_END();

  /// @brief Helper function for checking if two types are equality comparable
  /// @tparam Lhs The type of the left hand operand
  /// @tparam Rhs The type of the right hand operand
  /// @return Always returns @c false ; this overload is only selected if the types are not comparable
  template <typename Lhs, typename Rhs>
  static constexpr auto test(void*) noexcept -> bool {
    return false;
  }
};
}  // namespace comparison_traits_detail

/// Type trait to check if an instance of type @c Lhs can be compared to an
/// instance of type @c Rhs with the equality operator.
/// @tparam Lhs The type of the lhs
/// @tparam Rhs The type of the rhs. Defaults to @c Lhs
/// @return @c true if an instance of @c Lhs can be compared with an instance of
/// @c Rhs, @c false otherwise.
template <typename Lhs, typename Rhs = Lhs>
extern constexpr bool is_equality_comparable_v{
    //
    comparison_traits_detail::is_equality_comparable_helper::test<Lhs, Rhs>(0)
};

/// Type trait to check if an instance of type @c T can be compared to an
/// instance of type @c U with the equality operator without throwing an exception.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return @c true if an instance of @c T can be compared with an instance of
/// @c U without throwing, @c false otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_nothrow_equality_comparable_v = false;

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Checking for existence of comparison, not using it");
/// Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool
    is_nothrow_equality_comparable_v<T, U, constraints<std::enable_if_t<is_equality_comparable_v<T, U>>>> =
        noexcept(std::declval<T>() == std::declval<U>());
ARENE_IGNORE_END();

/// Type trait to check if an instance of type @c T can be compared to an
/// instance of type @c U with the inequality operator.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return @c true if an instance of @c T can be compared with an instance of
/// @c U, @c false otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_inequality_comparable_v = false;

/// Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool is_inequality_comparable_v<
    T,
    U,
    constraints<std::enable_if_t<std::is_convertible<decltype(std::declval<T>() != std::declval<U>()), bool>::value>>> =
    true;

/// Type trait to check if an instance of type @c T can be compared to an
/// instance of type @c U with the inequality operator without throwing an exception.
/// @tparam T The type of the lhs
/// @tparam U The type of the rhs. Defaults to @c T
/// @return @c true if an instance of @c T can be compared with an instance of
/// @c U without throwing, @c false otherwise.
template <typename T, typename U = T, typename = constraints<>>
extern constexpr bool is_nothrow_inequality_comparable_v = false;

/// Specialization for the case that @c T and @c U can be compared.
template <typename T, typename U>
extern constexpr bool
    is_nothrow_inequality_comparable_v<T, U, constraints<std::enable_if_t<is_inequality_comparable_v<T, U>>>> =
        noexcept(std::declval<T>() != std::declval<U>());

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_COMPARISON_TRAITS_HPP_
