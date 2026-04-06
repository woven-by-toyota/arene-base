// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ABSTRACT_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ABSTRACT_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {

/// @brief Type trait to detect if a type is an abstract class
/// @tparam T type may be an abstract class
///
template <class T>
extern constexpr bool is_abstract_v = __is_abstract(T);

/// @brief Type trait to detect if a type is an abstract class
/// @tparam T type may be an abstract class
///
template <class T>
class is_abstract : public bool_constant<is_abstract_v<T>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_ABSTRACT_HPP_
