// Copyright 2026, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_INTEGRAL_CONSTANT_LIKE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_INTEGRAL_CONSTANT_LIKE_HPP_

// IWYU pragma: private, include "arene/base/type_traits.hpp"
// IWYU pragma: friend "(arene/base(?!/tests)|stdlib/include/stdlib_detail)/.*"

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints/constraints.hpp"
#include "arene/base/stdlib_choice/enable_if.hpp"
#include "arene/base/stdlib_choice/integral_constant.hpp"
#include "arene/base/stdlib_choice/is_convertible.hpp"
#include "arene/base/stdlib_choice/is_default_constructible.hpp"
#include "arene/base/stdlib_choice/is_integral.hpp"
#include "arene/base/stdlib_choice/is_member_object_pointer.hpp"
#include "arene/base/stdlib_choice/is_same.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_cv.hpp"
// IWYU pragma: no_include "arene/base/stdlib_choice/remove_reference.hpp"
#include "arene/base/type_traits/comparison_traits.hpp"
#include "arene/base/type_traits/remove_cvref.hpp"

namespace arene {
namespace base {
namespace is_integral_constant_like_detail {

/// @brief implementation helper for @c arene::base::is_integral_constant_like_v
/// @tparam T The type to test
template <class T, class = arene::base::constraints<>>
extern constexpr bool is_integral_constant_like_v = false;

ARENE_IGNORE_START();
ARENE_IGNORE_ALL("-Wfloat-equal", "Floats are rejected without the use of their comparison.");
/// @brief implementation helper for @c arene::base::is_integral_constant_like_v
/// @tparam T The type to test
/// @note Use of @c T::value needs to be delayed to avoid hard errors with GCC 8
template <class T>
extern constexpr bool is_integral_constant_like_v<
    T,
    arene::base::constraints<                                                                   //
        std::enable_if_t<                                                                       //
            std::is_integral<remove_cvref_t<decltype(T::value)>>::value &&                      //
            !std::is_same<bool, remove_cvref_t<decltype(T::value)>>::value &&                   //
            std::is_convertible<T, decltype(T::value)>::value &&                                //
            is_equality_comparable_v<T, decltype(T::value)> &&                                  //
            !std::is_member_object_pointer<decltype(&T::value)>::value &&                       //
            std::is_default_constructible<T>::value &&                                          //
            std::integral_constant<bool, (static_cast<decltype(T::value)>(T()), true)>::value>  //
        >                                                                                       //
    > = std::integral_constant<bool, (T() == T::value)>::value &&                               //
        std::integral_constant<bool, (static_cast<decltype(T::value)>(T()) == T::value)>::value;
ARENE_IGNORE_END();

}  // namespace is_integral_constant_like_detail

/// @brief Backport for the C++26 exposition-only integral-constant-like concept
/// @tparam T The type to test
///
/// The concept integral-constant-like<T> specifies that all of the following
/// are true:
/// * is_integral_v<remove_cvref_t<decltype(T::value)>>
/// * !is_same_v<bool, remove_cvref_t<decltype(T::value)>>
/// * convertible_to<T, decltype(T::value)>
/// * equality_comparable_with<T, decltype(T::value)>
/// * bool_constant<T() == T::value>::value
/// * bool_constant<static_cast<decltype(T::value)>(T()) == T::value>::value
///
template <class T>
extern constexpr bool is_integral_constant_like_v = is_integral_constant_like_detail::is_integral_constant_like_v<T>;

/// @brief Backport for the C++26 exposition-only integral-constant-like concept
/// @tparam T The type to test
///
/// The concept integral-constant-like<T> specifies that all of the following
/// are true:
/// * is_integral_v<remove_cvref_t<decltype(T::value)>>
/// * !is_same_v<bool, remove_cvref_t<decltype(T::value)>>
/// * convertible_to<T, decltype(T::value)>
/// * equality_comparable_with<T, decltype(T::value)>
/// * bool_constant<T() == T::value>::value
/// * bool_constant<static_cast<decltype(T::value)>(T()) == T::value>::value
///
template <class T>
using is_integral_constant_like = std::integral_constant<bool, is_integral_constant_like_v<T>>;

}  // namespace base
}  // namespace arene

#endif  // INCLUDE_GUARD_ARENE_BASE_ARENE_BASE_TYPE_TRAITS_IS_INTEGRAL_CONSTANT_LIKE_HPP_
