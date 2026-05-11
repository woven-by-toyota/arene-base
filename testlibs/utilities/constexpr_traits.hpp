// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_CONSTEXPR_TRAITS_HPP_
#define INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_CONSTEXPR_TRAITS_HPP_

#include "arene/base/constraints.hpp"
#include "stdlib/include/tuple"
#include "stdlib/include/type_traits"

namespace testing {

namespace constexpr_traits_detail {

/// @brief Check if a type is default constructible in a constant expression
/// @tparam T The type to check
/// @return @c true if @c T is default constructible in a constant expression, @c false otherwise
template <typename T, typename = std::integral_constant<bool, std::is_default_constructible<T>::value>>
extern constexpr bool is_constexpr_default_constructible_v = false;

/// @brief Check if a type is default constructible in a constant expression
/// @tparam T The type to check
/// @return @c true if @c T is default constructible in a constant expression, @c false otherwise
template <typename T>
extern constexpr bool is_constexpr_default_constructible_v<T, std::integral_constant<bool, (T{}, true)>> = true;

/// @brief Helper function to invoke default and copy construction in a constant expression
/// @tparam T The type to check
/// @return @c true
///
/// Note: This is intended to be invoked in a constant expression to test if it compiles.
template <typename T>
constexpr auto test_constexpr_copy() -> bool {
  T const val{};
  T copy(val);
  std::ignore = copy;
  return true;
}

/// @brief Helper type to attempt to instantiate the copy construction in a constant expression
/// @tparam T The type to check
template <class T>
using uses_constexpr_default_and_copy_construction = std::integral_constant<bool, test_constexpr_copy<T>()>;

}  // namespace constexpr_traits_detail

/// @brief Check if a type is default constructible in a constant expression
/// @tparam T The type to check
///
/// Provides a member variable named @c value which is @c true if @c T is default constructible in a constant expression
/// or @c false otherwise
template <typename T>
struct is_constexpr_default_constructible
    : std::integral_constant<bool, constexpr_traits_detail::is_constexpr_default_constructible_v<T>> {};

/// @brief Check if a type is default constructible in a constant expression
/// @tparam T The type to check
///
/// @return @c true if @c T is default constructible in a constant expression or @c false otherwise
template <typename T>
extern constexpr bool is_constexpr_default_constructible_v =
    constexpr_traits_detail::is_constexpr_default_constructible_v<T>;

/// @brief Check if a type is default and copy constructible in a constant expression
/// @tparam T The type to check
/// @return @c true if @c T is default and copy constructible in a constant expression, @c false otherwise
///
/// @{
template <typename T, class = arene::base::constraints<>>
extern constexpr bool is_constexpr_default_and_copy_constructible_v = false;

template <typename T>
extern constexpr bool is_constexpr_default_and_copy_constructible_v<
    T,
    arene::base::constraints<
        std::enable_if_t<std::is_default_constructible<T>::value>,
        std::enable_if_t<std::is_copy_constructible<T>::value>>> =
    arene::base::substitution_succeeds<constexpr_traits_detail::uses_constexpr_default_and_copy_construction, T>;
/// @}

/// @brief Check if a type is copy constructible in a constant expression
/// @tparam T The type to check
///
/// Provides a member variable named @c value which is @c true if @c T is default and copy constructible in a constant
/// expression or @c false otherwise
template <typename T>
struct is_constexpr_default_and_copy_constructible
    : std::integral_constant<bool, is_constexpr_default_and_copy_constructible_v<T>> {};

/// @brief trait to determine if a test case is constexpr invocable
/// @tparam F nullary function object to invoke
///
/// @{
template <class F, class = std::integral_constant<bool, true>>
struct is_constexpr_invocable : std::integral_constant<bool, false> {};

template <class F>
struct is_constexpr_invocable<F, std::integral_constant<bool, (F{}(), true)>> : std::integral_constant<bool, true> {};

template <class F>
constexpr extern bool is_constexpr_invocable_v = is_constexpr_invocable<F>::value;
/// @}

}  // namespace testing

#endif  // INCLUDE_GUARD_ARENE_BASE_TESTLIBS_UTILITIES_CONSTEXPR_TRAITS_HPP_
