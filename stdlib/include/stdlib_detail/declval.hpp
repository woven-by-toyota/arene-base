// Copyright 2024, Toyota Motor Corporation
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_DECLVAL_HPP_
#define INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_DECLVAL_HPP_

// parasoft-begin-suppress CERT_CPP-DCL58-a-2 "Part of a standard library implementation"
// parasoft-begin-suppress AUTOSAR-A17_6_1-a-2 "Part of a standard library implementation"

// IWYU pragma: private, include <utility>
// IWYU pragma: friend "stdlib_detail/.*"

namespace std {

// parasoft-begin-suppress AUTOSAR-A2_7_3-a "False positive: there is a comment with @brief"
// parasoft-begin-suppress AUTOSAR-A2_7_3-b "False positive: there is a comment with @return"
// parasoft-begin-suppress AUTOSAR-M3_3_2-a-2 "False positive: inline function used in multiple translation units"
/// @brief A dummy function that is declared but not defined, with a user-specified return type, for use in unevaluated
/// expressions.
/// @tparam T The return type
/// @return An imaginary instance of the specified type if the type is a reference type, or as an rvalue of the
/// specified type otherwise.
template <typename T>
auto declval() noexcept -> T&&;
// parasoft-end-suppress AUTOSAR-A2_7_3-a
// parasoft-end-suppress AUTOSAR-A2_7_3-b
// parasoft-end-suppress AUTOSAR-M3_3_2-a-2

}  // namespace std

#endif  // INCLUDE_GUARD_ARENE_BASE_STDLIB_INCLUDE_STDLIB_DETAIL_DECLVAL_HPP_
