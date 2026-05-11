// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_VOID_T_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_VOID_T_HPP_

#include "arene/base/constraints.hpp"

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a-2 "False positive: the typedef *is* preceded by a comment with @brief"
/// @brief An alias for @c void that accepts any number of type parameters, used for substitution checks template
/// @tparam Args Any number of template parameters
template <class... Args>
using void_t = arene::base::void_t<Args...>;
// parasoft-end-suppress AUTOSAR-A2_7_3-a-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_VOID_T_HPP_
