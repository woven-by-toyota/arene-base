// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_TRIVIALLY_COPYABLE_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_TRIVIALLY_COPYABLE_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"
#include "stdlib/include/stdlib_detail/remove_volatile.hpp"

namespace std {

/// @brief Type trait to detect if an instance of @c T is trivially copyable
/// @tparam T The type of the value to check
/// @note The original C++14 standard says @c volatile qualified types are not trivially copyable. This was changed with
/// CWG issue 2094, which was accepted as a DR in 2016:
/// https://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#2094 This implementations matches the post-CWG2094
/// behaviour, which is consistent with C++17 and later too.
template <typename T>
extern constexpr bool is_trivially_copyable_v = __is_trivially_copyable(remove_volatile_t<T>);

/// @brief Type trait to detect if an instance of @c T is trivially copyable
/// @tparam T The type of the value to check
template <typename T>
class is_trivially_copyable : public bool_constant<is_trivially_copyable_v<T>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_TRIVIALLY_COPYABLE_HPP_
