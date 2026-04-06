// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_FLOATING_POINT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_FLOATING_POINT_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/remove_cv.hpp"

namespace std {
namespace is_floating_point_detail {
/// @brief Implementation constant for @c std::is_floating_point_v The value is @c true if @c Type is a floating point
/// type, @c false otherwise.
/// @tparam Type The type to check
/// @tparam Dummy A dummy parameter to allow specialization
template <typename Type, nullptr_t Dummy = nullptr>
extern constexpr bool is_floating_point_v = false;

/// @brief Implementation constant for @c std::is_floating_point_v The value is @c true if @c Type is a floating point
/// type, @c false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_floating_point_v<float, Dummy> = true;

/// @brief Implementation constant for @c std::is_floating_point_v The value is @c true if @c Type is a floating point
/// type, @c false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_floating_point_v<double, Dummy> = true;

// parasoft-begin-suppress AUTOSAR-A0_4_2-a-2 "long double use required for correct implementation"
/// @brief Implementation constant for @c std::is_floating_point_v The value is @c true if @c Type is a floating point
/// type, @c false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_floating_point_v<long double, Dummy> = true;
// parasoft-end-suppress AUTOSAR-A0_4_2-a-2

}  // namespace is_floating_point_detail

/// @brief A type trait to check if a type is floating point. The value is @c true if @c Type is a floating point type,
/// @c false otherwise.
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_floating_point_v = is_floating_point_detail::is_floating_point_v<remove_cv_t<Type>>;

/// @brief A type trait to check if a type is floating point. The class is derived from @c true_type if @c Type is a
/// floating point type, @c false_type otherwise.
/// @tparam Type The type to check
template <typename Type>
class is_floating_point : public bool_constant<is_floating_point_v<Type>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_FLOATING_POINT_HPP_
