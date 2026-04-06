// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ASSIGNABLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ASSIGNABLE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "arene/base/compiler_support/diagnostics.hpp"
#include "arene/base/constraints.hpp"
#include "stdlib/include/stdlib_detail/declval.hpp"
#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {

namespace is_assignable_detail {
/// @brief Type trait to detect if an instance of @c To is assignable to from arguments of @c From
/// @tparam To The type of the value to (try to) assign to
/// @tparam From The type of the value to (try to) assign from
template <typename To, typename From, typename = arene::base::constraints<>>
extern constexpr bool is_assignable_v = false;

/// @brief Type trait to detect if an instance of @c To is assignable to from arguments of @c From
/// @tparam To The type of the value to (try to) assign to
/// @tparam From The type of the value to (try to) assign from
template <typename To, typename From>
extern constexpr bool
    is_assignable_v<To, From, arene::base::constraints<decltype(std::declval<To>() = std::declval<From>())>> = true;

}  // namespace is_assignable_detail

/// @brief Type trait to detect if an instance of @c To is assignable to from arguments of @c From
/// @tparam To The type of the value to (try to) assign to
/// @tparam From The type of the value to (try to) assign from
template <typename To, typename From>
extern constexpr bool is_assignable_v = is_assignable_detail::is_assignable_v<To, From>;

/// @brief Type trait to detect if an instance of @c To is assignable to from arguments of @c From
/// @tparam To The type of the value to (try to) assign to
/// @tparam From The type of the value to (try to) assign from
template <typename To, typename From>
class is_assignable : public bool_constant<is_assignable_v<To, From>> {};

namespace is_nothrow_assignable_detail {
/// @brief Type trait to detect if an instance of @c To is assignable to from arguments of @c From without throwing
/// @tparam To The type of the value to (try to) assign to
/// @tparam From The type of the value to (try to) assign from
template <typename To, typename From, bool = is_assignable_v<To, From>>
extern constexpr bool is_nothrow_assignable_v = false;

ARENE_IGNORE_START();
ARENE_IGNORE_CLANG("-Wimplicit-int-conversion", "No actual conversion here, just a check");
ARENE_IGNORE_ARMCLANG("-Wimplicit-int-float-conversion", "No actual conversion here, just a check");
/// @brief Type trait to detect if an instance of @c To is assignable to from arguments of @c From without throwing
/// @tparam To The type of the value to (try to) assign to
/// @tparam From The type of the value to (try to) assign from
template <typename To, typename From>
extern constexpr bool is_nothrow_assignable_v<To, From, true> = noexcept(std::declval<To>() = std::declval<From>());
ARENE_IGNORE_END();

}  // namespace is_nothrow_assignable_detail

/// @brief Type trait to detect if an instance of @c To is assignable to from arguments of @c From without throwing
/// @tparam To The type of the value to (try to) assign to
/// @tparam From The type of the value to (try to) assign from
template <typename To, typename From>
extern constexpr bool is_nothrow_assignable_v = is_nothrow_assignable_detail::is_nothrow_assignable_v<To, From>;

/// @brief Type trait to detect if an instance of @c To is assignable to from arguments of @c From without throwing
/// @tparam To The type of the value to (try to) assign to
/// @tparam From The type of the value to (try to) assign from
template <typename To, typename From>
class is_nothrow_assignable : public bool_constant<is_nothrow_assignable_v<To, From>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ASSIGNABLE_HPP_
