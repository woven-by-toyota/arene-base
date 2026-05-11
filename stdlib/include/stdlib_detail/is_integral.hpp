// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_INTEGRAL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_INTEGRAL_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/cstddef.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/remove_cv.hpp"

// parasoft-begin-suppress AUTOSAR-A3_9_1-b-2 "Built in types must be explicitly specified to ensure correctness"

namespace std {
namespace is_integral_detail {
/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Type The type to check
/// @tparam Dummy A dummy parameter to allow specialization
template <typename Type, nullptr_t Dummy = nullptr>
extern constexpr bool is_integral_v = false;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_integral_v<char, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_integral_v<signed char, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_integral_v<unsigned char, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_integral_v<int, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_integral_v<unsigned, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
// NOLINTNEXTLINE(google-runtime-int)
extern constexpr bool is_integral_v<short, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
// NOLINTNEXTLINE(google-runtime-int)
extern constexpr bool is_integral_v<unsigned short, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
// NOLINTNEXTLINE(google-runtime-int)
extern constexpr bool is_integral_v<long, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
// NOLINTNEXTLINE(google-runtime-int)
extern constexpr bool is_integral_v<unsigned long, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
// NOLINTNEXTLINE(google-runtime-int)
extern constexpr bool is_integral_v<long long, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
// NOLINTNEXTLINE(google-runtime-int)
extern constexpr bool is_integral_v<unsigned long long, Dummy> = true;

// parasoft-begin-suppress AUTOSAR-A2_13_3-a-2 "wchar_t must be explicitly specified to ensure correctness"
/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_integral_v<wchar_t, Dummy> = true;
// parasoft-end-suppress AUTOSAR-A2_13_3-a-2

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_integral_v<char16_t, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_integral_v<char32_t, Dummy> = true;

/// @brief Implementation constant for @c std::is_integral_v The value is @c true if @c Type is an integral type, @c
/// false otherwise.
/// @tparam Dummy A dummy parameter to allow specialization
template <nullptr_t Dummy>
extern constexpr bool is_integral_v<bool, Dummy> = true;

}  // namespace is_integral_detail

/// @brief A type trait to check if a type is integral. The value is @c true if @c Type is an integral type, @c false
/// otherwise.
/// @tparam Type The type to check
template <typename Type>
extern constexpr bool is_integral_v = is_integral_detail::is_integral_v<remove_cv_t<Type>>;

/// @brief A type trait to check if a type is integral. The class is derived from @c true_type if @c Type is an integral
/// type, @c false_type otherwise.
/// @tparam Type The type to check
template <typename Type>
class is_integral : public bool_constant<is_integral_v<Type>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_INTEGRAL_HPP_
