// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_BASE_OF_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_BASE_OF_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <type_traits>
// IWYU pragma: friend "stdlib_detail/.*"

#include "stdlib/include/stdlib_detail/integral_constant.hpp"

namespace std {

/// @brief Type trait to detect if @c Base is a base class of @c Derived
/// @tparam Base The type which may be a base class
/// @tparam Derived The type which may be a derived class
///
template <typename Base, typename Derived>
extern constexpr bool is_base_of_v = __is_base_of(Base, Derived);

/// @brief Type trait to detect if @c Base is a base class of @c Derived
/// @tparam Base The type which may be a base class
/// @tparam Derived The type which may be a derived class
///
template <typename Base, typename Derived>
class is_base_of : public bool_constant<is_base_of_v<Base, Derived>> {};

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_IS_BASE_OF_HPP_
